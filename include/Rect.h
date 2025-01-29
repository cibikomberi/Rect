#ifndef RECT_H
#define RECT_H

#include <Arduino.h>
#include <functional>
#include <unordered_map>
#include <Client.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Update.h>


class Rect {
public:
    Rect();
    void begin(Client &client, String apiKey, String deviceId, String deviceVersion);
    void loop();
    void registerCallback(const std::string& key, std::function<void(float)> callback);
    void registerCallback(const std::string& key, std::function<void(std::string)> callback);
    void registerCommandProcessor(std::function<std::string(String)> callback);
    void put(String& id, float val);
    void put(String id, String val);
    void log(String val);

private:
    Client* internetClient;
    MQTTClient mqttClient;
    std::function<std::string(String)> commandProcessorCallback;
    std::unordered_map<std::string, std::function<void(float)>> floatHandlers;
    std::unordered_map<std::string, std::function<void(std::string)>> stringHandlers;

    void connectMqtt();
    void mqttCallback(String &topic, String &payload);
    void parseData(String payload);
    void checkUpdates(String targetVersion);
    void performOTA(Client& client, const char* host, const char* path, int port);
    String apiKey;
    String deviceId;
    String deviceVersion;
};

#endif
