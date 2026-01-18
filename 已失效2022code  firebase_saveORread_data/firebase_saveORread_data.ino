#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
  
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define API_KEY ""
#define DATABASE_URL ""

#define ledPin 10
#define ledPin2 12
#define sensorPin3 14

FirebaseData fbdo; //呼叫物件 當有數據節點發生變化的時候會處理數據 (處理數據傳輸)
FirebaseAuth auth; //身分驗證
FirebaseConfig config; //firebase需要進行配置所以可以用這行處理

unsigned long PresendDataMillis = 0;    // 宣告變數PresendDataMillis代表上一次儲存的時間    mills毫秒     prev上一頁
bool signupOK = false ;
int sensorData =0;
float voltage =0.0;


void setup() {
  

  config.api_key = API_KEY;                                                   //config deploy
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("singup ok");
    signupOK = true ;
   }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
   }

    config.token_status_callback = tokenStatusCallback;  //回傳token
    //網路裡麼是token? https://most.tw/posts/systemarchitect/tokenvsapikey202409/        token代表象徵的,符號(身分驗證,字元)
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
   
   // 要先有firebase資料完再啟動伺服器
    Serial.begin(115200);   
  Serial.print("開始連線到無線網路SSID:");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
  Server.begin();
  Serial.print("伺服器已啟動，http://");
  Serial.println(WiFi.localIP());

}

void loop() {
  if(Firebase.ready() && signupOK && (millis() -  PresendDataMillis >5000 || PresendDataMillis == 0 )){  //時間差大於5000或等於0就回傳資料

    PresendDataMillis  =  millis();
    //------- store sensor data to a RTDB
    sensorData = analogRead(sensorPin3);
    voltage = (float)analogReadMilliVolts(sensorPin3)/1000; //除以1000變成毫伏特存在db
    if(Firebase.RTDB.setInt(&fbdo, "Sensor/sensor_Data" , sensorData)){   //在firebase創建一個欄位"Sensor/sensor_Data" 然後上傳sensorData
        Serial.println(); 
        Serial.print(sensorData);
        Serial.print(" successfully saved to:" + fbdo.dataPath());
        Serial.println(" ("fbdo.dataType() + ") " );
    }else{
        Serial.println("FAILED:" + fbdo.errorReason());
    }

  }

}
