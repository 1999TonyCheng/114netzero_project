#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
  public:
    static void connect(const char* ssid, const char* password, IPAddress localIP, IPAddress gateway, IPAddress subnet);
};

#endif