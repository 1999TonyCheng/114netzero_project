#include "config.h"
#include "WiFiManager.h"
#include "RFControl.h"
#include "WebServerHandler.h"

// // WiFi 設定
IPAddress local_IP(STATIC_IP_1, STATIC_IP_2, STATIC_IP_3, STATIC_IP_4);
IPAddress gateway(GATEWAY_1, GATEWAY_2, GATEWAY_3, GATEWAY_4);
IPAddress subnet(SUBNET_1, SUBNET_2, SUBNET_3, SUBNET_4);

// 初始化控制模組
RFControl rfControl(5);
WebServerHandler webServer(&rfControl);

void setup() {
  Serial.begin(115200);
  Serial.println("Start Machine Working...");
  WiFiManager::connect(ssid, password, local_IP, gateway, subnet);
  webServer.begin();
}

void loop() {
  webServer.handle();
}