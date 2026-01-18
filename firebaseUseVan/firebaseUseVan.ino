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
#define WIFI_SSID "iPhone (55)"
#define WIFI_PASSWORD "00000000"

#define Web_API_KEY "AIzaSyDIzinnImNbmLm-j-5HmJEHPKlrz6Q67co"
#define DATABASE_URL "https://ainetzero-27c6f-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "qqwwee11566@gmail.com"
#define USER_PASS "user1234"

// User function
void processData(AsyncResult &aResult);

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// Timer variables for reading data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1500; // 10 seconds in milliseconds

// Variables to save values from the database
int intValue;
float floatValue;
String stringValue;
bool highValue;
bool lowValue;

//風扇腳位設定
int pin1 = 16; //
int pin2 = 17; //

void setup(){
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  pinMode(pin1, OUTPUT);//輸出電源給腳位,這裡電源是給馬達驅動器
  pinMode(pin2, OUTPUT);

  // Configure SSL client
  ssl_client.setInsecure();
  ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
}

void loop(){
  // Maintain authentication and async tasks
  app.loop();

  // Check if authentication is ready
  if (app.ready()){
    // Getting data every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;

      //aclient是異步同步函式  第二行是存放在db的路徑  第三行是異步同步資料驗證  第四行是遇到等待時是否放到queue裡面等  第五行是唯一標誌代表這個get
      // GET VALUES FROM DATABASE (using the callback async method method)
      // you can then get the values on the processData function as soon as the results are available
      /*Database.get(aClient, "/test/int", processData, false, "RTDB_GetInt");
      Database.get(aClient, "/test/float", processData, false, "RTDB_GetFloat");
      Database.get(aClient, "/test/string", processData, false, "RTDB_GetString");*/
      Database.get(aClient, "/van/high", processData, false, "RTDB_GetHigh");
      Database.get(aClient, "/van/low", processData, false, "RTDB_GetLow");
      //Database.get(aClient, "/van/low2", processData, false, "RTDB_GetLow2");

      //firebase操作esp32的風扇
      if (highValue==true && lowValue==false) { digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW);}
      if (highValue==false && lowValue==false) { digitalWrite(pin1, LOW); digitalWrite(pin2, LOW);}
      

      Serial.println("Requested data from /test/int, /test/float, and /test/string etc...");
    }
  }
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  // here you get the values from the database and save them in variables if you need to use them later
  if (aResult.available())    {
    // Log the task and payload
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());

    // Extract the payload as a String  所有aResult回傳結果存到payload裡面
    String payload = aResult.c_str();

    //aResult.uid()裡面的唯一標誌等於RTDB_GetHigh就取得他的資料存進變數highValue裡面
    if (aResult.uid() == "RTDB_GetHigh"){
      // Extract the value as an int
      highValue = payload.toInt();
      Firebase.printf("Stored intValue: %d\n", highValue);
    }

    if (aResult.uid() == "RTDB_GetLow"){
      // Extract the value as an int
      lowValue = payload.toInt();
      Firebase.printf("Stored intValue: %d\n", lowValue);
    }
/*
    if (aResult.uid() == "RTDB_GetLow2"){
      // Extract the value as an int
      low2Value = payload.toInt();
      Firebase.printf("Stored intValue: %d\n", low2);
    }*/

    /*/// Handle int from /test/int
    if (aResult.uid() == "RTDB_GetInt"){
      // Extract the value as an int
      intValue = payload.toInt();
      Firebase.printf("Stored intValue: %d\n", intValue);
    }
    // Handle float from /test/float
    else if (aResult.uid() == "RTDB_GetFloat"){
      // Extract the value as a float
      floatValue = payload.toFloat();
      Firebase.printf("Stored floatValue: %.2f\n", floatValue);
    }
        
    // Handle String from /test/string
    else if (aResult.uid() == "RTDB_GetString"){
      // Extract the value as a String
      stringValue = payload;
      Firebase.printf("Stored stringValue: %s\n", stringValue.c_str());
    }*/
  }
}