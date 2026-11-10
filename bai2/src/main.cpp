#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "TEN_WIFI_CUA_BAN";
const char* pass = "MAT_KHAU_WIFI";
const char* broker = "broker.hivemq.com";
const char* SV_ID = "nhom-25112214";

WiFiClient espClient;
PubSubClient mqtt(espClient);
String TOPIC_TELE;
unsigned long lastPub = 0;

float docNhiet() { return 28.0 + random(-10, 10) / 10.0; }
float docAm() { return 62.0 + random(-15, 15) / 10.0; }

void noiBroker() {
    while (!mqtt.connected()) {
        String clientId = "ESP32-Tele-" + String(WiFi.macAddress());
        Serial.print("Kết nối MQTT Broker...");
        if (mqtt.connect(clientId.c_str())) {
            Serial.println("Đã kết nối!");
        } else {
            Serial.printf("Lỗi rc=%d. Thử lại sau 5s\n", mqtt.state());
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi đã kết nối!");

    TOPIC_TELE = "aet2022/" + String(SV_ID) + "/telemetry";
    mqtt.setServer(broker, 1883);
}

void loop() {
    if (!mqtt.connected()) {
        noiBroker();
    }
    mqtt.loop();

    if (millis() - lastPub >= 2000) {
        lastPub = millis();
        JsonDocument doc;
        doc["temp"] = docNhiet();
        doc["hum"] = docAm();
        doc["uptime"] = millis() / 1000;
        doc["rssi"] = WiFi.RSSI();
        doc["id"] = SV_ID;

        char buffer[128];
        serializeJson(doc, buffer);
        mqtt.publish(TOPIC_TELE.c_str(), buffer);
        Serial.printf("Published to [%s]: %s\n", TOPIC_TELE.c_str(), buffer);
    }
}
