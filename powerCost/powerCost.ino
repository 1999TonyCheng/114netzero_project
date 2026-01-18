#include <ctime>
//#include <iostream>
//#include <iomanip>

double powerToUsage=.0;//使用度電  
double power;
double cost;//花的電費
double voltage;
double current=.0;
double TotalUsage=.0;


void setup() {
  Serial.begin(115200);

}

//P=VI 功率w=電壓v*電流a  
// 千瓦小時= P/1000*(x/3600)

void loop() {
  
  int Time=time(NULL);//經過的秒數

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
    
    voltage=5.0;//假設電壓5V
    current=15.0;//假設電流15a
    power = voltage* current;//P=VI
    TotalUsage = power/1000.0*(Time/3600.0);//總用電量 功率除以1000代表千瓦再乘上時間變成千瓦小時這邊把功率轉成實際用電量 **記得都要用小數點運算才算得出來
    //TotalUsage = TotalUsage + powerToUsage; 不用累加因為每秒都會成長一些 
    //powerUsage+=60.0;//每次加80度電 可直接用度電模擬計算電費
    Serial.print("功率:");
    Serial.print(power);
    Serial.print("W");
    Serial.print("  度電:");
    Serial.print(TotalUsage,5);  //　Serial.print(TotalUsage,10)　10代表顯示的小數位數
    Serial.print("瓩小時");
    Serial.print("  電費:");
    Serial.print(cost,5);
    Serial.println("元");

  delay(1000);
}
