#include <WiFi.h>
//#include <Arduino.h>
//#include <ESP32AnalogRead.h>//讀取類比訊號
//#include <analogWrite.h>//寫入類比訊號

//請修改以下參數--------------------------------------------
char ssid[] = ""; //請修改為您連線的網路名稱並在arduino改 連homeInternet
char password[] = ""; //請修改為您連線的網路密碼

WiFiServer server(80); //宣告伺服器位在80 port

int Relay_socket = 23; //宣告插座插孔1的繼電器在 GPIO 23
int A_1A=16; //L9110S馬達驅動板 A_1A腳 連接到ESP32的 GPIO16 
int A_1B=17; //L9110S馬達驅動板 A_1B腳 連接到ESP32的 GPIO17
int currentSensor=39 ;// 電流感測器訊號源設定在 ADC_GPIO39  偵測電流感測器類比數值轉換成電壓之後可用公式計算出電流
double total=.0;//用來加總類比訊號
double avg=.0;//用來平均類比訊號
double org_analog =.0;

//int Amp=18 ;//感測安培數值 設定在GPIO18
//long double totalAmp=.0 ;//Amp表示安培的基準值然後設定電流初值且用long double測的更精確   精度比較:float(佔4byte) < double(佔8byte) < long double(大於8byte)
//double orgAmp=0;
//double Mapping_Amp_value=.0;

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
  
  

  /*for(int i=1;i<=30;i++){
	totalAmp =  totalAmp+= analogRead(18);//把測到的電流加總起來
	delay(10);//間隔來避免偵測到相同值
  }*/
  
  
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

    for(int i=1;i<=30;i++){
      org_analog = analogRead(currentSensor);//orgAmp_analog代表原始交流電的類比訊號  original=原始 
      total+= org_analog;
      delay(10); //避免累加到重覆的值
    }
    avg=total/30.0; //算出平均值讓類比訊號更精確
    double vlotage =  org_analog*5/1023.0; // 實際電壓=原始電壓類比訊號*5/1023, 但一般要把正負所有範圍算進去例如:30A感測器範圍是+-30所以要乘上60才對,而這邊只有把感測一半範圍算進去否則可能當電壓改變的時候另一半範圍就感測不到了       //可以直接測esp32Vcc嗎?  
  double current = (vlotage-2.5)/0.066; //0.066為ACS712_30A可以感測到的每單位安培有的電壓   esp32無電流時的電壓為Vcc*0.5  要算出電流,ACS712電流轉換公式是  電流=(感測到電壓-無電流時電壓)/每1單位電流有的電壓  除以0.066就會得到實際電流
  Serial.print("交流電流:");
  Serial.println(current);

  }

}


