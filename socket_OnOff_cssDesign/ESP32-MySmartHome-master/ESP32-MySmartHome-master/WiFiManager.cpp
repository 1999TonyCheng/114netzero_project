#include "WiFiManager.h"

void WiFiManager::connect(const char* ssid, const char* password, IPAddress localIP, IPAddress gateway, IPAddress subnet) {
  WiFi.config(localIP, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.print("正在連線 WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi 連線成功，IP 位址為：" + WiFi.localIP().toString());
  delay(2000);
}