const int Pot=36;//可變電阻使用的腳位
int analogValue=0;

void setup() {
  Serial.begin(115200); //使用Serial一定要加否則印不出來
  pinMode(Pot,INPUT); //將Pot pin設置為類比輸入腳位
}

void loop() {
 
  analogValue=analogRead(Pot); //將Pot讀取到的值存到analogValue變數中。
  Serial.print("類比數值");
  Serial.println(analogValue);
  delay(100);

}
