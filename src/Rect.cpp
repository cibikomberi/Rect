#include "Rect.h"

Rect::Rect() {

}

void Rect::begin(Client &client, String apiKey, String deviceId, String deviceVersion) {
    this->apiKey = apiKey;
    this->deviceId = deviceId;
    this->deviceVersion = deviceVersion;

    internetClient = &client;
    mqttClient.begin("rect.local", 1883, client);
    mqttClient.onMessage([this](String &topic, String &payload) {
        this->mqttCallback(topic, payload);
    });
    connectMqtt();
}

void Rect::loop() {
    connectMqtt();
    mqttClient.loop();
}

void Rect::sync() {
    std::ostringstream oss;
    bool first = true; // To avoid adding an extra comma at the start

    for (const auto& pair : floatHandlers) {
        if (!first) oss << ", ";  // Add comma before every element except the first
        oss << pair.first;
        first = false;
    }
    for (const auto& pair : stringHandlers) {
        if (!first) oss << ", ";  
        oss << pair.first;
        first = false;
    }
    if (first) return;
    
    mqttClient.publish(("rect/" + String(deviceId) + "/sync").c_str(), oss.str().c_str(), false, 1);
}

void Rect::registerCallback(const std::string& key, std::function<void(float)> callback) {
    floatHandlers[key] = callback;
}

void Rect::registerCallback(const std::string& key, std::function<void(std::string)> callback) {
    stringHandlers[key] = callback;
}

void Rect::registerCommandProcessor(std::function<std::string(String)> callback) {
    commandProcessorCallback = callback;
}

void Rect::put(String& id, float val) {
    put(id, String(val));
}

void Rect::put(String id, String val) {
    mqttClient.publish(("rect/" + String(deviceId) + "/data").c_str(), "{\"" + id + "\":" + val + "}", false, 1);
}

void Rect::log(String val) {
    mqttClient.publish(("rect/" + String(deviceId) + "/log").c_str(),  val, false, 1);
}

void Rect::connectMqtt() {
    int i = 3;        
    while (!mqttClient.connected() && i > 0) {
        Serial.print("attempt");
        mqttClient.setWill(("rect/" + String(deviceId) + "/status").c_str(), "{\"status\":\"Offline\"}", true, 1);
        if (mqttClient.connect(deviceId.c_str())) {
            mqttClient.subscribe("rect/device/" + String(deviceId) + "/data");
            mqttClient.subscribe("rect/" + String(deviceId) + "/ota");
            mqttClient.subscribe("rect/device/" + String(deviceId) + "/command");
            sync();
        }
        mqttClient.publish(("rect/" + String(deviceId) + "/status").c_str(), "{\"status\":\"Online\"}", true, 1);
        i--;
    }
}

void Rect::mqttCallback(String &topic, String &payload) {
    Serial.print("Received message: ");
    Serial.print( topic);
    Serial.print( " - ");
    Serial.println(payload);
    if (topic.equals("rect/device/" + String(deviceId) + "/data")) {
        parseData(payload);
    } else if (topic.equals("rect/" + String(deviceId) + "/ota")) {
        checkUpdates(payload);
    } else if (topic.equals("rect/device/" + String(deviceId) + "/command")) {
        if (commandProcessorCallback) {
          std::string res = commandProcessorCallback(payload.c_str());
          if (res.length() > 0) {
            mqttClient.publish(("rect/" + String(deviceId) + "/log").c_str(), res.c_str(), false, 1);
          }
        }
    }
}

void Rect::parseData(String payload) {
    JsonDocument json;
    DeserializationError error = deserializeJson(json, payload);

    if (error) {
        return;
    }

    if (json["data"].is<float>() || json["data"].is<int>()) {
        if (floatHandlers.find(json["id"].as<std::string>()) != floatHandlers.end()) {
            floatHandlers[json["id"]](json["data"].as<float>());
        } else {
            mqttClient.publish(("rect/" + String(deviceId) + "/log").c_str(), "{\"type\": \"error\", No callback registered for " + json["id"].as<String>() + "}", false, 1);
        }
    } else {
        if (stringHandlers.find(json["id"].as<std::string>()) != stringHandlers.end()) {
            stringHandlers[json["id"]](json["data"].as<std::string>());
        } else {
            mqttClient.publish(("rect/" + String(deviceId) + "/log").c_str(), "{\"type\": \"error\", No callback registered for " + json["id"].as<String>() + "}", false, 1);
        }
    }
}

void Rect::checkUpdates(String targetVersion) {
    Serial.println("Checking for updates");
  if (targetVersion.equals(deviceVersion)) {
    return;
  }
  String url = "/thing/update/" + String(deviceId) + "?version=" + String(deviceVersion);
  performOTA(*internetClient, "rect.local", url.c_str(), 8080);
}

void Rect::performOTA(Client& client, const char* host, const char* path, int port) {
    if (!client.connect(host, port)) {
        return;
    }

    client.printf("GET %s HTTP/1.1\r\n", path);
    client.printf("Host: %s\r\n", host);
    client.println("User-Agent: ESP32-OTA\r\nConnection: close\r\n");

    while (client.connected() && !client.available()) {
        delay(10);
    }

    String line = client.readStringUntil('\n');
    if (!line.startsWith("HTTP/1.1 200")) {
        client.stop();
        return;
    }

    #ifdef ESP32
        int contentLength = -1;
    #elif defined(ESP8266)
        size_t contentLength = -1;
    #endif
    
    while (line != "\r") {
        line = client.readStringUntil('\n');
        if (line.startsWith("Content-Length: ")) {
            contentLength = line.substring(16).toInt();
        }
    }

    if (contentLength <= 0 || !Update.begin(contentLength)) {
        client.stop();
        return;
    }

    if (Update.writeStream(client) != contentLength || !Update.end()) {
        client.stop();
        return;
    }

    if (Update.isFinished()) {
        ESP.restart();
    }
    client.stop();
    connectMqtt();
} 

