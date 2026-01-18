#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <HTTPClient.h>
#include <ctime>  //C++的計時器有許多
 //#include <Arduino.h>
//#include <ESP32AnalogRead.h>//讀取類比訊號
//#include <analogWrite.h>//寫入類比訊號

//請修改以下參數--------------------------------------------
char ssid[] = ""; //請修改為您連線的網路名稱並在arduino改 連homeInternet
char password[] = ""; //請修改為您連線的網路密碼
WiFiServer server(80); //宣告伺服器位在80 port
String url = "http://api.thingspeak.com/update?api_key=360Q3XUOAFRF587H&field1=vlotage&field2=current";		//上傳數據到tinkspeak													 

#define Web_API_KEY "" //REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY
#define DATABASE_URL "" //REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL
#define USER_EMAIL "" //REPLACE_WITH_FIREBASE_PROJECT_EMAIL_USER
#define USER_PASS "" //REPLACE_WITH_FIREBASE_PROJECT_USER_PASS



//firebase非同步用   函式要放前面這樣後面程式碼才能呼叫
void processData(AsyncResult &aResult) {
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}


// Authentication 以下程式碼行使用專案 API 金鑰、專案使用者電子郵件和密碼建立一個驗證物件。
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;  //這將建立一個名為 app 的 FirebaseApp 物件，該物件指向 Firebase 應用程式。
//以下程式碼設定了與 Firebase 即時資料庫互動的非同步通訊框架
WiFiClientSecure ssl_client;//step1 使用庫建立一個SSL物件
using AsyncClient = AsyncClientClass;//建立一個"AsyncClient"物件
AsyncClient aClient(ssl_client);//AsyncClient建立"aClient"來進行ssl_client
RealtimeDatabase Database;//建立了一個名為 Database 的RealtimeDatabase 物件，該對象代表 Firebase 即時資料庫。






int Relay_socket = 23; //宣告插座插孔1的繼電器在 GPIO 23
int A_1A=16; //L9110S馬達驅動板 A_1A腳 連接到ESP32的 GPIO16 
int A_1B=17; //L9110S馬達驅動板 A_1B腳 連接到ESP32的 GPIO17


int currentSensor=39 ;// 電流感測器訊號源設定在 ADC_GPIO39  偵測電流感測器類比數值轉換成電壓之後可用公式計算出電流
double total=.0;//用來加總類比訊號
double avg=.0;//用來平均類比訊號
double org_analog =.0;
double Vout=.0; //vlotage
//double current=.0;  也有用到



//電費花費變數
double powerToUsage=.0;//使用度電  
double power;
double cost;//花的電費
double voltage;
double current=.0;
double TotalUsage=.0;


//firebase用變數
//然後創建變數，並將資料保存到資料庫。
// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds  1秒=1000mill  台電是15min
// Variables to send to the database
int intValue = 0;
float floatValue = 0.01;
String stringValue = "";




//中斷電流用變數
time_t tt;     //建立time_t變數(長整數形態)以記錄時間。單位為秒。 用來上傳tingkspeak

//異常中斷用變數
 int pot=36; //可變電阻的設定在 ADC_GPIO36 偵測電壓的類比訊號
double current_analog=.0;//原始訊號
int firstTime=0;//前
int lastTime=0;//後
int tempTime=0;//暫存時間
bool currentFlag=0;

//int Amp=18 ;//感測安培數值 設定在GPIO18
//long double totalAmp=.0 ;//Amp表示安培的基準值然後設定電流初值且用long double測的更精確   精度比較:float(佔4byte) < double(佔8byte) < long double(大於8byte)
//double orgAmp=0;
//double Mapping_Amp_value=.0;

void ReadSensor(){
  /*這個是學思達那個公式: (原始-平均基準)*電流感測器正負範圍/訊號範圍 ex (2500-2045 )*10/4095
  for(int i=1;i<=500;i++){
      org_analog = analogRead(currentSensor);//orgAmp_analog代表原始交流電的類比訊號  original=原始 
      total+= org_analog;
      delay(10); //避免累加到重覆的值
    }
    avg=total/500.0; //算出平均值讓類比訊號更精確*/
  for(int i = 0; i < 1000; i++) { //偵測1000次累加後再計算平均值
    Vout = (Vout + ( (5.0/4095.0) * analogRead(currentSensor)));  //累加(5.00/4095)*A0。 公式= 原始訊號* VCC /板子訊號大小
    delay(1);
    }
    Vout = Vout /1000;//把電壓取平均
    current =(Vout-2.5)/0.066;  // 電流=(平均電壓-2.5Vcc)/電流匯率      5A_0.183  20A_0.1  30A_0.066 
    //0.066為ACS712_30A可以感測到的每單位安培有的電壓

    //vlotage =  org_analog*5/4095.0; // 實際電壓=原始電壓類比訊號*5/1023, 但一般要把正負所有範圍算進去例如:30A感測器範圍是+-30所以要乘上60才對,而這邊只有把感測一半範圍算進去否則可能當電壓改變的時候另一半範圍就感測不到了       //可以直接測esp32Vcc嗎?  
    //


}


//將資料上傳ThinkSpeak
void UploadData(){ 
  
  //建立一個網頁
  HTTPClient http;  //Searil啟動網頁也可以

  //把sensor數值寫入網址
  String url1= url+"&field1="+(double)Vout+"&field2="+(double)current;

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



void current_interupt(){
  
  
  ReadSensor();

  /*double pot_analog = analogRead(pot);//讀取電壓(分壓)的類比訊號
  Serial.println();//換行
  Serial.print("  可變電阻原始訊號:");
  Serial.print(pot_analog);*/
  double current_analog = analogRead(currentSensor);
  Serial.print("  電流原始訊號:");
  Serial.print(current_analog);
  Serial.print("  電流基準值:");
  Serial.print(currentSensor);//  某次使用經驗公式計時一分鐘觀測出來的avg 但之後電流會浮動就不準了所以用計時器比較精準*/
  Serial.print("  轉換後實際電流:");
  Serial.print(current);
  Serial.print("安培  ");
  Serial.print(time(NULL)); //1秒=1000毫秒 +delay去換算 可time*0.01和delay設10來觀察數值 
  Serial.print("秒");
  //delay(1000);

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
    Serial.println("  開始累積時間");
  }
  if(currentFlag==1 && (lastTime-tempTime)%15==0 ){ //這變代表電流一直持續過大 當電流flag持續大於某數值且持續累積超過15秒就關掉電器了
      digitalWrite(A_1A, LOW);  digitalWrite(A_1B, LOW); //把風扇關掉
      Serial.println("  超過15秒啟動保護關掉風扇");
      
      //tempTime=0;//如果電器關閉就重置time
      //lastTime=0;
  }
  if(current<2.5){ //回到正常電流就把flag重置
    currentFlag=0;
    Serial.println("  電流小於2.5");
    //tempTime=0;//如果電器關閉就重置time
      //lastTime=0;
  }
}


void powerCost(){
    
    ReadSensor(); //讀取電流
    
    double current0=.0;
    int Time=time(NULL);//經過的秒數
    voltage=Vout;                           //讀取電壓值
    current0=current;                       //讀取實際電流值存進current0
    power = voltage* current0;//P=VI
    TotalUsage = power/1000.0*(Time/3600.0);//總用電量 功率除以1000代表千瓦再乘上時間變成千瓦小時這邊把功率轉成實際用電量 **記得都要用小數點運算才算得出來

    

  if (TotalUsage <= 120) {
        cost = TotalUsage * 1.68;
    } else if (TotalUsage <= 330 && TotalUsage >=121) {
        cost =  120 * 1.68 + (TotalUsage - 120) * 2.45;
    } else if (TotalUsage <= 500 && TotalUsage >=331) {
        cost = 120 * 1.68 + 210 * 2.45 + (TotalUsage - 330) * 3.7;
    } else if (TotalUsage <= 700 && TotalUsage >=501) {
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + (TotalUsage - 500) * 5.04;
    } else if (TotalUsage <= 1000 && TotalUsage >=701){
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + 200 * 5.04 + (TotalUsage - 700) * 6.24;
    }else if (TotalUsage >=1001){
        cost = 120 * 1.68 + 210 * 2.45 + 170 * 3.7 + 200 * 5.04 + 300 * 6.24 + (TotalUsage - 1000) *8.46;
    
    }
    /*
    Serial.print("功率:");
    Serial.print(power);
    Serial.print("W");
    Serial.print("  度電:");
    Serial.print(TotalUsage,5);  //　Serial.print(TotalUsage,10)　10代表顯示的小數位數
    Serial.print("瓩小時");
    Serial.print("  電費:");
    Serial.print(cost,5);
    Serial.println("元");*/
    

}


void firebaseStoredata(){
  ReadSensor();
  powerCost();//才能讀取度電 電費
  
  /*for(int i=1 ;i<=10;i++){
delay(1500);
  }*/

  // Maintain authentication and async tasks 我們使用的 Firebase 函式庫採用非同步工作方式並使用回呼函數，所以要添加app.loop持續偵測
  while(!app.ready()){ //用while才能確保app.loop連線了
    app.loop(); 
  }
  
  for(int i=1 ; i<=1000 ; i++){
      app.loop(); 
    
  }

  // Check if authentication is ready app.ready() 指令檢查 Firebase 驗證是否完成並準備就緒，以便我們可以繼續執行其他 Firebase 操作（例如寫入資料庫）。
  if (app.ready()){ 
    // Periodic data sending every 10 seconds  這邊是否超過10秒,"sendInterval"也可以自己設定時間傳到資料庫
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){  //interval 間隔間隙
      // Update the last send time
      lastSendTime = currentTime;
      
      //寫入firebase語法:
      //Database.set<型別>(aClient, "路徑例如/test/string", 要入的變數名稱 , 非同步=使用processData, "RTDB_Send_String"是const String &uid);
      // aClient是非同步客戶端物件，它管理與 Firebase 的網路連線。
      //const String &uid ：任務的唯一標識符，用於追蹤回呼函數中的特定操作。這有助於區分 processData() 函數中的多個非同步任務。

      //顯示電費 度電 電壓 電流?  current Vout  TotalUsage  cost
        
      // send a string
      /*stringValue = "value_" + String(currentTime);
      Database.set<String>(aClient, "/test/string", stringValue, processData, "RTDB_Send_String");*/
      // send an int
      Database.set<int>(aClient, "/test/int", intValue, processData, "RTDB_Send_Int");
      intValue++; //increment intValue in every loop*/

      // send a float
      //double currentValue =   current;        // 0.01 + random (0,100);
      Database.set<float>(aClient, "/currentSensor/current", current, processData,"RTDB_Send_current");  //這邊第三行直接把值傳給firebase

      //int VoutValue =  Vout;         
      Database.set<float>(aClient, "/currentSensor/Vout", Vout, processData, "RTDB_Send_Vout");

      //int TotalUsageValue =  TotalUsage;         
      Database.set<float>(aClient, "/power/TotalUsage", TotalUsage, processData, "RTDB_Send_TotalUsage");

      //int costValue =  cost;         
      Database.set<float>(aClient, "/power/cost", cost, processData, "RTDB_Send_cost");

    }
  }

}



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

  // *****切記:這個腳位在接其他感測器信號需要改成input，否則一般感測器信號無法承受3~5V的電壓基本上會直接燒壞*******
  
  //esp32電壓輸出給驅動器讓風扇轉動
  pinMode(A_1A,OUTPUT); 
  pinMode(A_1B,OUTPUT);
  delay(1000);//延遲1000毫秒先讓電壓電流穩定再測量電壓的類比訊號    (物理量)
  
  

  /*for(int i=1;i<=30;i++){totalAmp =  totalAmp+= analogRead(18);//把測到的電流加總起來delay(10);//間隔來避免偵測到相同值}*/


//這六行是firebase用
   // Configure SSL client  配置 SSL 客戶端參數。
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);
  // Initialize Firebase  初始化or重新配置Firebase的參數
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask"); //非同步..
  app.getApp<RealtimeDatabase>(Database); //把之前定義的物件設定成預設呼叫的
  Database.url(DATABASE_URL);
  
  
}



void loop()
{
    
  

  firebaseStoredata();
  powerCost();
  current_interupt();

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
	
	// 設計 client 上的瀏覽器網頁格式，包括顏色、字型大小、有無邊框、字元等
	client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
	client.println("<link rel=\"icon\" href=\"data:,\">");// 
	//CSS style的on/off按鈕 
	client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");// 可自由調整按鈕顏色和方框大小
	client.println(".button { background-color: orange; border: none; color: white; padding: 1px 20px;");
	client.println("text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}");
	client.println(".button2 {background-color: #555666;}</style></head>");
	
    client.println("<html><head><meta charset='utf-8'></head>");
    client.println("<br>");
    client.println("<h1>智慧淨零管理系統</h1>");
    //HTML超連結指令
    client.println("<a>插孔   </a>");
    client.println("<a href='/Relay_socket=ON'><button class=\"button\">開啟on</button> </a>" );
    client.println("<a href='/Relay_socket=OFF'><button class=\"button button2\">關閉off</button></a><br>");
    client.println("<a>風扇   </a>");
    client.println("<a href='/Relay2_motor=ON'><button class=\"button\">開啟on</button> </a>");
    client.println("<a href='/Relay2_motor=OFF'><button class=\"button button2\">關閉off</button></a><br>");
    client.println("<iframe src=https://thingspeak.mathworks.com/channels/3062257/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15 height=250 width=418 ></iframe><br>");
	/*digitalWrite(A_1B,HIGH); 
    digitalWrite(A_1A,LOW);*/
    /*client.println(" ");   
    client.println(" ");   
    client.println(" ");   
    */            
                    
    
    client.println("</html>");
    //-------------網頁的html部份結束--------------
    
    //取得使用者輸入的網址
    String request = client.readStringUntil('\r');
    Serial.println(request);
    //判斷超連結指令
    //網址內包含Relay_socket=ON就開啟插座孔1，如果Relay_socket=OFF，關閉插座孔1
    if (request.indexOf("Relay_socket=ON") >= 0) { digitalWrite(Relay_socket, HIGH); }
    if (request.indexOf("Relay_socket=OFF") >= 0) { digitalWrite(Relay_socket, LOW); }

    if (request.indexOf("Relay2_motor=ON") >= 0) { digitalWrite(A_1B,HIGH); digitalWrite(A_1A,LOW);} //風扇需要兩個output搭配
    if (request.indexOf("Relay2_motor=OFF") >= 0) { digitalWrite(A_1B,LOW); digitalWrite(A_1A,LOW); }
    Serial.println("完成");
    client.stop();//停止連線
	
	//避免一些變數宣告在loop一直被重置造成邏輯錯誤
	
	//Mapping_Amp_value = (orgAmp-totalAmp)*10/1023  ;  //*1000可以變豪安培先乘避免數字不見
    //Serial.print("原始數值Amp:");
/*  這個是減基準值的那個公式
    for(int i=1;i<=30;i++){
      org_analog = analogRead(currentSensor);//orgAmp_analog代表原始交流電的類比訊號  original=原始 
      total+= org_analog;
      delay(10); //避免累加到重覆的值
    }
    avg=total/30.0; //算出平均值讓類比訊號更精確
    */

                            /*  錯誤公式會因為會算出-37安培
                              double vlotage =  org_analog*5/4095.0; // 實際電壓=原始電壓類比訊號*5/1023, 但一般要把正負所有範圍算進去例如:30A感測器範圍是+-30所以要乘上60才對,而這邊只有把感測一半範圍算進去否則可能當電壓改變的時候另一半範圍就感測不到了       //可以直接測esp32Vcc嗎?  
                              double current = (vlotage-2.5)/0.066; //0.066為ACS712_30A可以感測到的每單位安培有的電壓   esp32無電流時的電壓為Vcc*0.5  要算出電流,ACS712電流轉換公式是  電流=(感測到電壓-無電流時電壓)/每1單位電流有的電壓  除以0.066就會得到實際電流
                              Serial.print("交流電流:");
                              Serial.println(current);*/

  }

}


