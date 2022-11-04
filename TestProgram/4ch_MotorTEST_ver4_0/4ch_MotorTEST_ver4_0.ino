//
// 4ch Motor TEST program
//           Ver 4.0 Mar.12.2019
//           By Saeki Hidekazu
// すべてのモーターを同時に動かす
// モーターは時計方向(CW)に回転
//

#include "motorDRV4.h"
#include "LED_TEST2.h"

int power = 50;
int ledPin = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  motorInit();
  LED_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  LED_R(HIGH);
  motorCh1(power);
  motorCh2(power);
  motorCh3(power);
  motorCh4(power);
  delay(5000);
  LED_R(LOW);
  motorCh1(0);
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
  delay(500);
  //power = -power;
  LED_B(HIGH);
  motorCh1(power);
  motorCh2(power);
  motorCh3(power);
  motorCh4(power);
  delay(5000);
  LED_B(LOW);
  motorCh1(0);
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
  delay(500);
  //power = -power;
}
