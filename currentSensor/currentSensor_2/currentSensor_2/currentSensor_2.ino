//PWN腳位用來控制高低電位  類比輸入腳位來感測類比訊號傳給esp32 類比輸出腳位來傳類比訊號給元件  
//可參考arduino IDE analog範例


 int currentSensor=39 ;// 電流感測器訊號源設定在 ADC_GPIO39  偵測電流的類比數值轉之後可用公式計算出電流
 int pot=36; //可變電阻的設定在 ADC_GPIO36 偵測電壓的類比訊號
double current_analog=.0;//原始訊號
double totalAnalog=.0;//全部的原始訊號
double avg=.0;//原始訊號平均
//double temp=0;
//int doubleu密集度不同

//另一種算法  double avgAnalog=.0;double totalAnalog=.0 ;//Amp表示安培的基準值然後設定電流初值且用long double測的更精確   精度比較:float(佔4byte) < double(佔8byte) < long double(大於8byte)  

//double orgAmp=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  

  delay(1000);//先讓電穩定1000毫秒後再讀取值 
  for(int i=1 ; i<=30 ; i++){     //500次會很好 用計數器每時段更新會更好 目前問題是算完一次平均就沒辦法再持續更新平均值了後面就會有誤差 。讓電流變精準的方法 統計學平均 還有RMS QR分解等等  
      current_analog = analogRead(currentSensor);
      totalAnalog+= current_analog;
  }    
  avg=totalAnalog/30.0;  
}

void loop() {
  // put your main code here, to run repeatedly:
    
    /*Serial.print("原始數值Amp:"); 
      Serial.println(orgAmp);//
      delay(50);*/

  // 幫訊號取平均值之後算出的安培 有其他線性回歸、統計學的方式會更精確 例如RMS QR分解
  
                                                                                                                                        //另一種算法double avgAnalog=totalAnalog/30;//把全部類比訊號取平均當作要計算的基準點  avgAnalog*60/1023.0; 60就是ACS712的感測範圍 把正負30A的數字取絕對值做加總再除以類比訊號的上限1023最後會得到 每1訊號有多少訊號  //可用org_analog直接算出物理量 但不精確
 
  //int double波型不同
  //!!注意!! analogread一定要放在迴圈裡面才會一直更新數值  
  //電流感測器就是偵測電壓的類比訊號所以可以算出實際電壓再算出實際店流 只要有感測器能感測電壓類比訊號都可以這樣算
  
                                                                                                          //???double vlotage =  org_analog*5/4095; //  算法實際電壓=原始電壓類比訊號*5Vcc/4095,    // 別種算法->但一般要把正負所有範圍算進去例如:30A感測器範圍是+-30所以要乘上60才對,而這邊只有把感測一半範圍算進去否則可能當電壓改變的時候另一半範圍就感測不到了   //0~1023是別的板子    //可以直接測esp32Vcc嗎?  
                                                                                                          //???double current = (vlotage-2.5)/0.066; //0.066為ACS712_30A可以感測到的每單位安培有的電壓   esp32無電流時的電壓為Vcc*0.5  要算出電流,ACS712電流轉換公式是  電流=(感測到電壓-無電流時電壓)/每1單位電流有的電壓  除以0.066就會得到實際電流
  double pot_analog = analogRead(pot);//讀取電壓(分壓)的類比訊號
  //double voltage = pot_analog/4095.0*5.0; ////應該要用電壓感測器牽涉到硬體怎麼設計的 //把類比訊號轉成實際電壓 可變電阻間距0~5所以乘5 如果是25就乘以25  esp32腳位有0~4995(12位元組)所以除以4095  ps:有些板子腳位只有0~1023(10位元組) 
  double current_analog = analogRead(currentSensor);//orgAmp_analog代表原始交流電的類比訊號  original=原始
  double current = (current_analog-3200.0)*60.0/4096.0;//先減基準值才會剩下+-浮動值(多了多少?), 再乘跨距(-30~30 要看感測器能測的範圍) ,最後看要不要換單位 //不同單位計算時要先換成一樣單位
  //因為電流會浮動 所以直接減2048.0不可行   用經驗觀察法算出平均是3200
  //(current_analog-avg)

  Serial.print("  可變電阻原始訊號:");
  Serial.print(pot_analog);
  /*Serial.print("  轉換後實際電壓(分壓):");
  Serial.print(voltage);
  Serial.print("伏特");*/
  Serial.print("  電流原始訊號:");
  Serial.print(current_analog);
  Serial.print("  電流基準值:");
  Serial.print("3200.0");//avg
  Serial.print("  轉換後實際電流:");
  Serial.print(current);
  Serial.println("安培");


  //double current = (org_analog-avg)*60/4095;
  /*Serial.print("  原始訊號:");
  Serial.print(org_analog);
  Serial.print("  實際電壓:");
  Serial.print(vlotage);
  Serial.print("  交流電流:");
  Serial.println(current); */
  delay(100);
  /*for(int i=1 ; i<=300 ; i++){  //用來平均原始訊號比較int double
      temp+=org_analog;
      delay(10);
  }
  Serial.print("  原始數據平均:");
  Serial.println(temp/300);*/
  //Serial.print("  30A感測器的原始訊號除以5:");
  //Serial.println(org_analog/5);*/

  
  
  
 

    
}
