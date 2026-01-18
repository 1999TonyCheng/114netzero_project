#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <WebServer.h>
#include "RFControl.h"

class WebServerHandler {
  public:
    WebServerHandler(RFControl* rf);
    void begin();
    void handle();

  private:
    WebServer server;
    RFControl* rfControl;

    void handleRoot();
    void handleLight();
    void handleFan();
};

#endif