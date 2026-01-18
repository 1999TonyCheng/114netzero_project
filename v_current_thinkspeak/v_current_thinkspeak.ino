#include <WiFi.h>
#include <HTTPClient.h>
 #include <ctime>  //C++的計時器有許多

// 請修改以下參數-----------------------------------------------------------
char ssid[] = "";
char password[] = "";
// 請修改為自己的 API Key，並將 https 改為 http
String url = "http://api.thingspeak.com/update?api_key=";
int currentSensor = 39;             // 電流感測訊號腳位
double org_analog=.0;
double total=.0;
double avg=.0;
double vlotage=.0;
double current=.0;
time_t tt;     //建立time_t變數(長整數形態)以記錄時間。單位為秒。 
WiFiServer server(80); //宣告伺服器位在80 port
/*clock_t first_time, last_time;      //建立clock_t變數(長整數形態)以記錄時間，以毫秒(0.001秒)為單位
double timeTotal=.0;//計算時間差的總和
double cal_time=.0;//計算時間差*/
//----------------------------------------------------------------

void ReadSensor(){
  for(int i=1;i<=30;i++){
      org_analog = analogRead(currentSensor);//orgAmp_analog代表原始交流電的類比訊號  original=原始 
      total+= org_analog;
      delay(10); //避免累加到重覆的值
    }
    avg=total/30.0; //算出平均值讓類比訊號更精確
    vlotage =  org_analog*5/1023.0; // 實際電壓=原始電壓類比訊號*5/1023, 但一般要把正負所有範圍算進去例如:30A感測器範圍是+-30所以要乘上60才對,而這邊只有把感測一半範圍算進去否則可能當電壓改變的時候另一半範圍就感測不到了       //可以直接測esp32Vcc嗎?  
   current = (vlotage-2.5)/0.066; //0.066為ACS712_30A可以感測到的每單位安培有的電壓
}

//將資料上傳ThinkSpeak
void UploadData(){ 
  
  //建立一個網頁
  HTTPClient http;  //Searil啟動網頁也可以

  //把sensor數值寫入網址
  String url1= url+"&field1="+(double)vlotage+"&field2="+(double)current;

  //提交網址
  http.begin(url1);

  //讀取網頁內容
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK){
    String payload = http.getString();
    Serial.print("網頁內容=");
    Serial.println(payload);
  } 
  else{
     Serial.println("網路傳送失敗");
  }

  //關閉網頁
  http.end(); 
  
}

void setup()
{
  Serial.begin(115200);
  Serial.print("開始連線到無線網路 SSID:");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
  server.begin();
  Serial.print("伺服器已啟動，http://");
  Serial.println(WiFi.localIP());
}

void loop(){

  //想法是用總時間%20 只要是20秒的倍數就是上傳資料因為thinkspeak api有時間限制
  tt=time(NULL);  //指派tt為「從1970年1月1日0時0分0秒」到「現在」的時間差，單位為秒。
  //Serial.print("秒數:");
  //Serial.println(tt);
  //delay();
  if(tt%20==0){ //只要是20秒的倍數就做這件事:上傳thinkspeak
      ReadSensor();
    UploadData();
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
    client.println("<a href='/Gled=ON'>exsample link</a><br>");
    client.println("<iframe src=https://exsample height=252 width=450 ></iframe><br>");
    client.println("<iframe src=https://exsample height=252 width=450 ></iframe>");
    client.println("</html>");
    //-------------網頁的html部份結束--------------
    //取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    //判斷超連結指令
    //網址內包含Gled=ON，就開啟綠燈，如果Gled=OFF，關閉綠燈
    //if (request.indexOf("Gled=ON") >= 0) { digitalWrite(Gled, HIGH); }
    //if (request.indexOf("Gled=OFF") >= 0) { digitalWrite(Gled, LOW); }
    Serial.println("完成");
    client.stop();//停止連線
  }


}

    /* 這個程式上傳時間會不固定  且麵包板有燒焦味
void loop()
{
  first_time=clock(); //first_time在這邊為「fro loop」開始的時間

  if(timeTotal*1000 >=20){//大於20秒就做這件事:上傳thinkspeak
    ReadSensor();
  UploadData();
  timeTotal =0; //把timeTotal重新計時
  }
  //delay(20100); thinkspeak上傳後需要休息 20 秒 ，但如果在loop裡面休息20秒其他功能就跟著暫停20秒了,這就不是我們想要的結果

  last_time=clock();    //last_time在這邊為「for loop」結束的時間
  cal_time =last_time -first_time; //時間差
  timeTotal+=cal_time; //一直把時間差做加總 以毫秒為單位(0.001秒)
  
  }
*/
