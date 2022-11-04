//
// 4ch Motor TEST program
//           Ver 4.2 Jul.21.2020
//           By Saeki Hidekazu
// すべてのモーターを段階的に同時に動かす
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
  int i;
  LED_R(HIGH);
  digitalWrite(ledPin, HIGH);
  for (i = 0; i < 10; i++) {
    motorCh1(i * 10);
    motorCh2(i * 10);
    motorCh3(i * 10);
    motorCh4(i * 10);
    delay(1000);
  }
  LED_B(HIGH);
  digitalWrite(ledPin, LOW);
  for (i = 0; i < 10; i++) {
    motorCh1(100 - i * 10);
    motorCh2(100 - i * 10);
    motorCh3(100 - i * 10);
    motorCh4(100 - i * 10);
    delay(1000);
  }
  LED_B(LOW);
  motorCh1(0);
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
  delay(500);
  LED_R(HIGH);
  digitalWrite(ledPin, HIGH);
  for (i = 0; i < 10; i++) {
    motorCh1(-i * 10);
    motorCh2(-i * 10);
    motorCh3(-i * 10);
    motorCh4(-i * 10);
    delay(1000);
  }
  LED_B(HIGH);
  digitalWrite(ledPin, LOW);
  for (i = 0; i < 10; i++) {
    motorCh1(-(100 - i * 10));
    motorCh2(-(100 - i * 10));
    motorCh3(-(100 - i * 10));
    motorCh4(-(100 - i * 10));
    delay(1000);
  }
  LED_B(LOW);
  motorCh1(0);
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
  delay(500);

}
