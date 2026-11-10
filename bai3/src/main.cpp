#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "TEN_WIFI_CUA_BAN";
const char* pass = "MAT_KHAU_WIFI";
const char* broker = "broker.hivemq.com";
const char* SV_ID = "nhom-25112214";

const int LED_PIN = 14; // GPIO14 theo slide TH4
WiFiClient espClient;
PubSubClient mqtt(espClient);

String TOPIC_TELE;
String TOPIC_CMD;
unsigned long lastPub = 0;

void callback(char* topic, byte* payload, unsigned int length) {
    String msg = "";
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.printf("Nhận lệnh từ Topic [%s]: %s\n", topic, msg.c_str());

    if (msg == "1" || msg == "on") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("-> BẬT LED GPIO14");
    } else if (msg == "0" || msg == "off") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("-> TẮT LED GPIO14");
    }
}

void noiBroker() {
    while (!mqtt.connected()) {
        String clientId = "ESP32-BiDir-" + String(WiFi.macAddress());
        Serial.print("Kết nối MQTT Broker...");
        if (mqtt.connect(clientId.c_str())) {
            Serial.println("Đã kết nối!");
            mqtt.subscribe(TOPIC_CMD.c_str());
            Serial.printf("Đã Subscribe topic: %s\n", TOPIC_CMD.c_str());
        } else {
            Serial.printf("Lỗi rc=%d. Thử lại sau 5s\n", mqtt.state());
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi đã kết nối!");

    TOPIC_TELE = "aet2022/" + String(SV_ID) + "/telemetry";
    TOPIC_CMD = "aet2022/" + String(SV_ID) + "/cmd/led";

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(callback);
}

void loop() {
    if (!mqtt.connected()) {
        noiBroker();
    }
    mqtt.loop();

    if (millis() - lastPub >= 2000) {
        lastPub = millis();
        JsonDocument doc;
        doc["temp"] = 28.5 + random(-5, 5) / 10.0;
        doc["hum"] = 65.0 + random(-10, 10) / 10.0;
        doc["uptime"] = millis() / 1000;
        doc["rssi"] = WiFi.RSSI();
        doc["ledState"] = digitalRead(LED_PIN);

        char buffer[128];
        serializeJson(doc, buffer);
        mqtt.publish(TOPIC_TELE.c_str(), buffer);
    }
}
