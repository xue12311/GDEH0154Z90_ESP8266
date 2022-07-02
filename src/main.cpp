#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "../lib/WiFiManager/WiFiManager.h"

/**
 * 连接 wifi
 * @return true 连接成功 false 连接失败
 */
bool onAutoConnectWiFi();



void setup() {
    Serial.begin(9600);
    //wifi 连接成功
    if(onAutoConnectWiFi()){

    }
}

void loop() {
}

/**
 * 自动连接 wifi
 */
bool onAutoConnectWiFi() {
    Serial.println("WiFi 连接中...");
    WiFiManager wifiManager;
    bool res = wifiManager.autoConnect("ESP8266 e-Paper", "12345678");
    if (!res) {
        Serial.println("WiFi 连接失败");
        delay(2000);
        //重置并重试
        ESP.restart();
        delay(5000);
        return res;
    }
    Serial.println("");
    Serial.println("Esp8266 WiFi 连接成功 : " + WiFi.SSID());
    Serial.println("IP 地址 : " + WiFi.localIP().toString());
    return res;
}