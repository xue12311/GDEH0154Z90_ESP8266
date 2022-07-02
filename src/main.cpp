#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "../lib/WiFiManager/WiFiManager.h"

WiFiManager wifiManager;

void setup() {
    Serial.begin(9600);
    wifiManager.autoConnect("ESP8266 e-Paper", "12345678");
    Serial.println("");
    Serial.println("Esp8266 WiFi 连接成功 : " + WiFi.SSID());
    Serial.println("IP 地址 : " + WiFi.localIP().toString());
}

void loop() {
}