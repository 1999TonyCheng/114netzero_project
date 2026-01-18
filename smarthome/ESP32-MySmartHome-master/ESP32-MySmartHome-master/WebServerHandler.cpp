#include "WebServerHandler.h"

WebServerHandler::WebServerHandler(RFControl* rf) : server(80), rfControl(rf) {}

void WebServerHandler::begin() {
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/light", HTTP_GET, [this]() { handleLight(); });
  server.on("/fan", HTTP_GET, [this]() { handleFan(); });
  server.begin();
  Serial.println("HTTP 伺服器已啟動");
}

void WebServerHandler::handle() {
  server.handleClient();
}

void WebServerHandler::handleRoot() {
  String floor = "3F";
  if (server.hasArg("floor")) {
    floor = server.arg("floor");
    if (!rfControl->getCommandForFloor(floor)) floor = "3F";
  }

  String html = R"rawliteral(
    <html lang="zh-Hant"><head>
    <meta charset='utf-8'>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>控制面板</title>
    <style>
      body { font-family: Arial, sans-serif; padding: 20px; margin: 0; background-color: #f8f9fa; text-align: center; }
      h2 { margin-top: 30px; color: #333; }
      .button-row { display: flex; flex-wrap: wrap; justify-content: center; gap: 12px; margin: 15px 0; }
      button { 
        flex: 1 0 40%; max-width: 140px; padding: 15px; font-size: 18px; border-radius: 10px; border: none; background-color: #4CAF50; color: white;
        box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2); cursor: pointer; transition: 0.2s; 
      }
      button:hover { background-color: #45a049; }
      @media (min-width: 600px) { button { flex: 1 0 20%; } }
    </style>
    <script>
      function sendCommand(url) {
        fetch(url).then(res => res.text()).then(txt => {
          document.getElementById('log').innerText = txt;
        });
      }
      function switchFloor(floor) {
        window.location.href = "/?floor=" + encodeURIComponent(floor);
      }
    </script>
    </head><body>
    <h2>溫叨的智慧家居</h2>
  )rawliteral";

  html += "<h2>位置 " + floor + "</h2>";
  html += "<button onclick=\"sendCommand('/light?floor=" + floor + "&times=1')\">電燈開關</button><br>";

  html += "<h3>🌀 風扇強度</h3>";
  html += "<div class=\"button-row\">";
  html += "<button onclick=\"sendCommand('/fan?floor=" + floor + "&level=0')\">關閉</button>";
  for (int i = 1; i <= 6; i++) {
    html += "<button onclick=\"sendCommand('/fan?floor=" + floor + "&level=" + String(i) + "')\">強度 " + String(i) + "</button>";
  }
  html += "</div><br><br>";
  html += "<pre id='log'>狀態回傳會顯示在這裡</pre>";

  // 加入樓層切換按鈕
  html += "<div class=\"button-row\">";
  auto cmds = rfControl->getAllCommands();
  for (int i = 0; i < rfControl->getCommandCount(); ++i) {
    if (cmds[i].floor != floor) {
      html += "<button onclick=\"switchFloor('" + cmds[i].floor + "')\">切換到 " + cmds[i].floor + "</button>";
    }
  }

  html += "</div></body></html>";
  server.send(200, "text/html", html);
}

void WebServerHandler::handleLight() {
  String floor = server.arg("floor");
  int times = server.arg("times").toInt();
  if (rfControl->sendLight(floor, times)) {
    server.send(200, "text/plain", "已控制樓層 " + floor + " 的燈 " + String(times) + " 次");
  } else {
    server.send(400, "text/plain", "未知的樓層");
  }
}

void WebServerHandler::handleFan() {
  String floor = server.arg("floor");
  int level = server.arg("level").toInt();
  if (rfControl->sendFan(floor, level)){
    server.send(200, "text/plain", floor + " 風扇設定為 Lv" + String(level));
  } else {
    server.send(400, "text/plain", "未知的樓層");
  }
}