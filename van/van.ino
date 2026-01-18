#include <WiFi.h>
//請修改以下參數--------------------------------------------
char ssid[] = ""; //請修改為您連線的網路名稱
char password[] = ""; //請修改為您連線的網路密碼

int pin1 = 16; //
int pin2 = 17; //
WiFiServer server(80); //宣告伺服器位在80 port

void setup()
{
  Serial.begin(115200);
  Serial.print("開始連線到無線網路SSID:");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
  server.begin();
  Serial.print("伺服器已啟動，http://");
  Serial.println(WiFi.localIP());
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
 
}

void loop()
{
  //宣告一個連線
  WiFiClient client = server.available();
  if (client) {
    //有人連入時
    Serial.println("使用者連入");
    //-------------網頁的html部份開始--------------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html><head><meta charset='utf-8'></head>");
    client.println("<br>");
    client.println("<h1>ESP32 Web Server</h1>");
    //HTML超連結指令
    client.println("<a href='/Gled=ON'>測試風扇</a><br>");
    client.println("<a href='/Gled=OFF'>測試風扇</a><br>");
    client.println("</html>");
    //-------------網頁的html部份結束--------------
    //取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    //判斷超連結指令
    //網址內包含Gled=ON，就開啟綠燈，如果Gled=OFF，關閉綠燈
    if (request.indexOf("Gled=ON") >= 0) { digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW);}
    if (request.indexOf("Gled=OFF") >= 0) { digitalWrite(pin1, LOW); digitalWrite(pin2, LOW);}
    Serial.println("完成");
    client.stop();//停止連線
  }
}
