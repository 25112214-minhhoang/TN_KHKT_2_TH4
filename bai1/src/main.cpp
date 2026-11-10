#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "TEN_WIFI_CUA_BAN";
const char* pass = "MAT_KHAU_WIFI";

WebServer server(80);

float docNhiet() { return 27.5 + random(-10, 10) / 10.0; }
float docAm() { return 60.0 + random(-20, 20) / 10.0; }

void handleSensor() {
    JsonDocument doc;
    doc["temp"] = docNhiet();
    doc["hum"] = docAm();
    doc["uptime"] = millis() / 1000;
    doc["id"] = "25112214"; // MSSV của em
    doc["rssi"] = WiFi.RSSI();

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi đã kết nối!");
    Serial.print("REST API Endpoint: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/api/sensor");

    server.on("/api/sensor", HTTP_GET, handleSensor);
    server.begin();
}

void loop() {
    server.handleClient();
}
