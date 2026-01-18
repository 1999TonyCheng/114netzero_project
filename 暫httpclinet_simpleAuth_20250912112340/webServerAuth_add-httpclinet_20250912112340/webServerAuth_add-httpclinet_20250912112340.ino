#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char *ssid = "";
const char *password = "";

WiFiServer server(8080); //宣告伺服器位在80 port
//WebServer server(80);//宣告server這個物件


const char *username = "user";
const char *userpassword = "123";//只要改密碼驗證就重置了

void handleRoot(){
  if(!server.authenticate(username,userpassword)){ //驗證用戶是否登陸
    return server.requestAuthentication(); //請求用戶進行登陸驗證
  }
  server.send(200); //登陸後200代表回應成功
  //server.send(200,"text/plain","Login successful!"); //登陸成功則顯示app內容  這邊是顯示Login successful
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());

  server.on("/",handleRoot);

  server.begin();
  Serial.println("Web server started");

}

void loop() {
  //server.handleClient(); 

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
    client.println("<h1>智慧淨零管理系統</h1>");
    }

}
