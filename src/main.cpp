#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <ePaperDisplay.h>

#include "../lib/WiFiManager/WiFiManager.h"
#include "../lib/pubsubclient/src/PubSubClient.h"

//MQTT 服务器 地址
const char *mqtt_server = "test.ranye-iot.net";


//开发板 mac 地址
const String str_device_mac_address = WiFi.macAddress();

//绑定 Esp8266 设备 主题名称
const char *topic_name_bind_esp8266_device = "ranye/iot/init/bind_esp8266_device";

//刷新 电子屏 主题名称
const String str_topic_device_refresh_epaper_display =
        "ranye/iot/epaper/refresh_epaper_display_" + str_device_mac_address;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//wifi 连接成功
bool isWiFiConnectionSucceeded = false;

/**
 * 连接 wifi
 * @return true 连接成功 false 连接失败
 */
bool onAutoConnectWiFi();

/**
 * 初始化 MQTT 服务器
 */
void initMqttServer();

/**
 * 连接 MQTT 服务器
 * @return true 连接成功 false 连接失败
 */
bool onConnectMqttServer();

/**
 * 发送 mqtt 消息 绑定 开发板设备
  * @return  true 发送成功  false 发送失败
  */
bool onPushMqttMessage();

/**
 * 订阅 MQTT 主题
 */
void onSubscribeMqttTopic();

/**
 *  收到信息后的回调函数
  * @param topic  主题名称
  * @param payload  消息内容
  * @param length 消息长度
  */
void receiveCallback(char *topic, byte *payload, unsigned int length);


/**
 * 处理 收到的 MQTT 消息
 * @param topic_name 主题名称
 * @param message 消息内容
  * @param length 消息长度
  */
void onHandleMqttMessage(String topic_name, String message, unsigned int length);

void setup() {
    Serial.begin(9600);
    //初始化 电子标签
    initEPaperDisplay();
    //wifi 连接成功
    if (onAutoConnectWiFi()) {
        //初始化 mqtt
        initMqttServer();
        //连接 MQTT 服务器
        if (onConnectMqttServer()) {
            onSubscribeMqttTopic();
            onPushMqttMessage();
        }
    }
}

void loop() {
    // WiFi 连接成功
    if (isWiFiConnectionSucceeded) {
        if (mqttClient.connected()) { // 如果开发板成功连接服务器
            //保持心跳
            mqttClient.loop();
        } else {
            delay(3000);
            //重新连接
            if (onConnectMqttServer()) {
                onSubscribeMqttTopic();
                onPushMqttMessage();
            }
        }
    }
}

/**
 * 订阅 主题
 */
void onSubscribeMqttTopic() {
    //刷新 电子屏 主题名称
    char mSubTopicRefreshEPaperDisplay[str_topic_device_refresh_epaper_display.length() + 1];
    strcpy(mSubTopicRefreshEPaperDisplay, str_topic_device_refresh_epaper_display.c_str());
// 通过串口监视器输出是否成功订阅主题以及订阅的主题名称
    if (mqttClient.subscribe(mSubTopicRefreshEPaperDisplay)) {
        Serial.println("订阅主题成功 : " + str_topic_device_refresh_epaper_display);
    } else {
        Serial.println("订阅主题失败 : " + str_topic_device_refresh_epaper_display);
    }
}

/**
 * 发送 mqtt 消息 绑定 开发板设备
 */
bool onPushMqttMessage() {
    // 如果开发板成功连接服务器
    if (mqttClient.connected()) {
        // 发送 mqtt 消息 绑定 开发板设备
        if (mqttClient.publish(topic_name_bind_esp8266_device, str_device_mac_address.c_str())) {
            Serial.println("发送 mqtt 消息 (绑定开发板设备) 成功 : " + str_device_mac_address);
            return true;
        } else {
            Serial.println("发送 MQTT 消息 (绑定开发板设备) 失败");
            return false;
        }
    } else {
        Serial.println("开发板未连接 MQTT服务器");
        return false;
    }
}

/**
 * 初始化 MQTT 
 * 
 */
void initMqttServer() {
// 设置MQTT服务器和端口号
    mqttClient.setServer(mqtt_server, 1883);
// 设置MQTT订阅回调函数
    mqttClient.setCallback(receiveCallback);
}

// 收到信息后的回调函数
void receiveCallback(char *topic, byte *payload, unsigned int length) {
    String str_payload;
    for (int i = 0; i < length; i++) {
        str_payload += (char) payload[i];
    }
    onHandleMqttMessage(String(topic), str_payload, length);
}

/**
 * 处理 收到的 MQTT 消息
 * @param topic_name 主题名称
 * @param message 消息内容
  * @param length 消息长度
  */
void onHandleMqttMessage(String topic_name, String message, unsigned int length) {
    Serial.print("收到 MQTT 消息 [ " + topic_name + " ] : " + message);
    Serial.println("");
    if (topic_name.equals(str_topic_device_refresh_epaper_display)) {
        if (message.equals("1")) {
            //刷新 电子屏
            setImageEPaperDisplay();
        } else if (message.equals("0")) {
            //清除 电子屏
            setCleanEPaperDisplay();
        }
    }
}

/**
 * 连接 MQTT 服务器
 */
bool onConnectMqttServer() {
    // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
    String clientId = "e-paper-esp8266-" + str_device_mac_address;
    // 连接MQTT服务器
    if (mqttClient.connect(clientId.c_str())) {
        // 连接成功
        Serial.println("MQTT连接成功");
        Serial.println("服务器地址: " + String(mqtt_server));
        Serial.println("客户端Id: " + clientId);
        return true;
    } else {
        // 连接失败
        Serial.println("MQTT连接失败, 错误码: " + String(mqttClient.state()));
        return false;
    }
}

/**
 * 自动连接 wifi
 */
bool onAutoConnectWiFi() {
    Serial.println("WiFi 连接中...");
    WiFiManager wifiManager;
    bool res = wifiManager.autoConnect("ESP8266 e-Paper", "12345678");
    isWiFiConnectionSucceeded = res;
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