/*
   ロボット側(SofrwareSerial)から来た
   データをそのままxbee(HardwareSerial)
   に送り出す。
   23.nov.2020 by H.Saeki
*/

#include <SoftwareSerial.h>
SoftwareSerial xbeeSerial(10, 11); // メインCPUに繋がるシリアルポート

int led = 4;
void setup() {
  Serial.begin(9600); // Xbee向けのシリアルポートを初期化
  xbeeSerial.begin(9600); //　メインCPU向けのシリアルポートを初期化
  
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.println(" Ready!");  // Xbeeに文字を出力
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);
  digitalWrite(led, LOW);   // turn the LED off (LOW is the voltage level)

}

void loop() {
  char buf[4];
  xbeeSerial.listen();  // メインCPUからのデータを受信する
  if (xbeeSerial.available() > 0) {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.write(xbeeSerial.read());  // メインCPUから来たデータをXbeeに送り出す。
    digitalWrite(led, LOW);   // turn the LED off (LOW is the voltage level)
  }
}
