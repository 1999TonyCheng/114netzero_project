#include <WiFi.h>
#include <ctime>
#include <iostream>
//請修改以下參數--------------------------------------------
char ssid[] = ""; //請修改為您連線的網路名稱
char password[] = ""; //請修改為您連線的網路密碼


int currentSensor=39 ;// 電流感測器訊號源設定在 ADC_GPIO39  偵測電流的類比數值轉之後可用公式計算出電流
 int pot=36; //可變電阻的設定在 ADC_GPIO36 偵測電壓的類比訊號
double current_analog=.0;//原始訊號
int B_1A=16;
int B_1B=17;
int firstTime=0;//前
int lastTime=0;//後
int tempTime=0;//暫存時間
bool currentFlag=0;

//double totalAnalog=.0;//全部的原始訊號
//double avg=.0;//原始訊號平均

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
  pinMode(B_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);
}

void loop()
{
  double pot_analog = analogRead(pot);
  double current_analog = analogRead(currentSensor);
  double current = (current_analog-3200.0)*60.0/4095.0;
  Serial.println();//換行
  Serial.print("  可變電阻原始訊號:");
  Serial.print(pot_analog);
  /*Serial.print("  轉換後實際電壓(分壓):");
  Serial.print(voltage);
  Serial.print("伏特");*/
  /*Serial.print("  電流原始訊號:");
  Serial.print(current_analog);
  Serial.print("  電流基準值:");
  Serial.print("3200.0");//  某次使用經驗公式計時一分鐘觀測出來的avg 但之後電流會浮動就不準了所以用計時器比較精準*/
  Serial.print("  轉換後實際電流:");
  Serial.print(current);
  Serial.print("安培 ");
  Serial.print(time(NULL)*0.01); //1秒=1000毫秒 +delay去換算
  Serial.print("秒");
  delay(10);

  //電流平均器


  if( current >=2.5){  //電流大於2.5就記錄起始時間和設定flag=1   //末-初可以實作定時開關
    firstTime= time(NULL);
    currentFlag=1;
    Serial.print("  偵測到電流過大"); 
  }
  if(tempTime==0){//把起始位置暫存避免一直被刷新
    tempTime = firstTime;
  }
  
  if(currentFlag==1){ //當flag等於1就會一值刷新並記錄時間
    lastTime= time(NULL);
    Serial.print("  開始累積時間");
  }
  if(currentFlag==1 && (lastTime-tempTime)/15==1 ){ //這變代表電流一直持續過大 當電流flag持續大於某數值且持續累積超過15秒就關掉電器了
      digitalWrite(B_1A, LOW);  digitalWrite(B_1B, LOW); //把風扇關掉
      Serial.print("  超過15秒啟動保護關掉風扇");
      
      //tempTime=0;//如果電器關閉就重置time
      //lastTime=0;
  }
  if(current<2.5){ //回到正常電流就把flag重置
    currentFlag=0;
    Serial.print("  電流小於2.5");
    //tempTime=0;//如果電器關閉就重置time
      //lastTime=0;
  }
    


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
    client.println("<a href='/montor=ON'>開啟馬達風扇</a><br>");
    client.println("<a href='/montor=OFF'>關閉馬達</a><br>");
    client.println("</html>");
    //-------------網頁的html部份結束--------------
    //取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    //判斷超連結指令
    //開關
    if (request.indexOf("montor=ON") >= 0) { digitalWrite(B_1A, LOW);  digitalWrite(B_1B, HIGH);}
    if (request.indexOf("montor=OFF") >= 0) { digitalWrite(B_1A, LOW);  digitalWrite(B_1B, LOW);  }
    
    
    Serial.println("完成");
    client.stop();//停止連線
  }
}