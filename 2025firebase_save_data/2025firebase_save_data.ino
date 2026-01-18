/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
*********/
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// Network and Firebase credentials
#define WIFI_SSID "iPhone (55)"//REPLACE_WITH_YOUR_SSID
#define WIFI_PASSWORD "00000000"//REPLACE_WITH_YOUR_PASSWORD

#define Web_API_KEY "AIzaSyDIzinnImNbmLm-j-5HmJEHPKlrz6Q67co" //REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY
#define DATABASE_URL "https://ainetzero-27c6f-default-rtdb.asia-southeast1.firebasedatabase.app/" //REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL
#define USER_EMAIL "qqwwee11566@gmail.com" //REPLACE_WITH_FIREBASE_PROJECT_EMAIL_USER
#define USER_PASS "user1234" //REPLACE_WITH_FIREBASE_PROJECT_USER_PASS

// User function
void processData(AsyncResult &aResult);

// Authentication 以下程式碼行使用專案 API 金鑰、專案使用者電子郵件和密碼建立一個驗證物件。
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;  //這將建立一個名為 app 的 FirebaseApp 物件，該物件指向 Firebase 應用程式。
//以下程式碼設定了與 Firebase 即時資料庫互動的非同步通訊框架
WiFiClientSecure ssl_client;//step1 使用庫建立一個SSL物件
using AsyncClient = AsyncClientClass;//建立一個"AsyncClient"物件
AsyncClient aClient(ssl_client);//AsyncClient建立"aClient"來進行ssl_client
RealtimeDatabase Database;//建立了一個名為 Database 的RealtimeDatabase 物件，該對象代表 Firebase 即時資料庫。


//然後創建變數，並將資料保存到資料庫。
// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10 seconds in milliseconds  1秒=1000mill  台電是15min
// Variables to send to the database
int intValue = 0;
float floatValue = 0.01;
String stringValue = "";


void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  
  
  // Configure SSL client  配置 SSL 客戶端參數。
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);
  
  // Initialize Firebase  初始化or重新配置Firebase的參數
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask"); //非同步..
  app.getApp<RealtimeDatabase>(Database); //把之前定義的物件設定成預設呼叫的
  Database.url(DATABASE_URL);
}

void loop(){
  // Maintain authentication and async tasks 我們使用的 Firebase 函式庫採用非同步工作方式並使用回呼函數，所以要添加app.loop持續偵測
  app.loop();
  // Check if authentication is ready app.ready() 指令檢查 Firebase 驗證是否完成並準備就緒，以便我們可以繼續執行其他 Firebase 操作（例如寫入資料庫）。
  if (app.ready()){ 
    // Periodic data sending every 10 seconds  這邊是否超過10秒,也可以自己設定時間傳到資料庫
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;
      
      //寫入firebase語法:
      //Database.set<型別>(aClient, "路徑例如/test/string", 要入的變數名稱 , 非同步=使用processData, "RTDB_Send_String"是const String &uid);
      // aClient是非同步客戶端物件，它管理與 Firebase 的網路連線。
      //const String &uid ：任務的唯一標識符，用於追蹤回呼函數中的特定操作。這有助於區分 processData() 函數中的多個非同步任務。

      // send a string
      stringValue = "value_" + String(currentTime);
      Database.set<String>(aClient, "/test/string", stringValue,  processData,"RTDB_Send_String");//
      // send an int
      Database.set<int>(aClient, "/test/int", intValue,  processData,"RTDB_Send_Int");//
      intValue++; //increment intValue in every loop

      // send a string
      floatValue = 0.01 + random (0,100);
      Database.set<float>(aClient, "/test/float", floatValue, processData, "RTDB_Send_Float");//
    }
  }
}

/*Process the Async Results 處理非同步結果
最後， processData() 函數會記錄非同步 Firebase 操作的結果。
*/
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