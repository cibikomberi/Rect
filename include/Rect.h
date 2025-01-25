#ifndef RECT_H
#define RECT_H

#include <Arduino.h>
#include <functional>
#include <unordered_map>
#include <Client.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Update.h>

#ifndef RECT_API_KEY
#error "API_KEY is not defined. Please define it using #define API_KEY \"your_api_key_here\""
#endif
#ifndef RECT_DEVICE_ID
#error "API_KEY is not defined. Please define it using #define API_KEY \"your_api_key_here\""
#endif

class Rect {
public:
    Rect();
    void begin(Client &client);
    void registerCallback(const std::string& key, std::function<void(float)> callback);
    void registerCallback(const std::string& key, std::function<void(std::string)> callback);
    void registerCommandProcessor(std::function<std::string(std::string)> callback);
    void put(String& id, float val);
    void put(String id, String val);

private:
    Client* internetClient;
    MQTTClient mqttClient;
    std::function<std::string(std::string)> commandProcessorCallback;
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
