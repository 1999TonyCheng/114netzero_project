#include <WiFi.h>
#include <WebServer.h>


const char *ssid = "";
const char *password = "";

WebServer server(80);//宣告server這個物件

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
  server.handleClient(); 

  

}
