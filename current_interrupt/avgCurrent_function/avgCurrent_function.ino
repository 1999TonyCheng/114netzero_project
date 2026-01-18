#include <ctime>

int currentSensor=39 ;// 電流感測器訊號源設定在 ADC_GPIO39  偵測電流的類比數值轉之後可用公式計算出電流
 int pot=36; //可變電阻的設定在 ADC_GPIO36 偵測電壓的類比訊號
double current_analog=.0;//原始訊號
int firstTime=0;//前
int lastTime=0;//後
int tempTime=0;//暫存時間
bool currentFlag=0;
double currentTotal=.0;
double currentAvg =.0;
int count=0;

void setup() {
  Serial.begin(115200);

}

void loop() {

  double pot_analog = analogRead(pot);
  double current_analog = analogRead(currentSensor);

/*這邊可能是錯的 不該原始訊號-原始訊號平均，應該原始訊號-上下限的中間值
  //for 或delay會讓程式時間複雜度變大所以改用計時器
  currentTotal+=current_analog;
  ++count; //這邊是每單位毫秒就壘加一次
  if(time(NULL)%5==0 ){ //這邊是每15秒就做平均
  currentAvg=currentTotal/count;
  currentTotal=0;//重置避免壘加到上次的值                                                                                                 //可以一直累加直到overflow?? 不行 最後平均就會固定
  count=0;  //每X秒重新計算 ,count也可以改成直接代數字
  
  }*/
  

  double current = (current_analog-currentAvg)*60.0/4095.0*1000.0; // 因為esp32類比訊號腳位最大是4096(2^12 )要乘以60才能顯示+-30 否則就最多只顯示1而已 ex4095/4095
  Serial.println();//換行
  Serial.print("  可變電阻原始訊號:");
  Serial.print(pot_analog);
  /*Serial.print("  轉換後實際電壓(分壓):");
  Serial.print(voltage);
  Serial.print("伏特");*/
  Serial.print("  電流原始訊號:");
  Serial.print(current_analog);
  Serial.print("  電流基準值:");
  Serial.print(currentAvg);//  某次使用經驗公式計時一分鐘觀測出來的avg 但之後電流會浮動就不準了所以用計時器比較精準
  Serial.print("  轉換後實際電流:");
  Serial.print(current);
  Serial.print("豪安培 "); //豪安培 =安培*1000
  Serial.print("第");
  Serial.print(time(NULL)); //1秒=1000毫秒 
  Serial.print("秒");
  delay(1000);


}
