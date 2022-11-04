//
// 4ch Motor TEST program
//           Ver 2.1 Mar.12.2019
//           By Saeki Hidekazu
// CH1からCH4までの4個のモータを順番に動かす。
//

#include "motorDRV4.h"		// モーター制御のサブプログラムを読み込む
#include "LED_TEST2.h"

const int power = 30;		// モーターの速度を30%にする
const int ledPin = 13;

void setup() {		// 初期化する
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Start");
  motorInit();
  LED_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  motorCh1(power);				// CH1のモーターを30％の速さで動かす
  LED_R(HIGH);
  Serial.println("CH1 ");
  delay(5000);						// 5秒待つ
  motorCh1(0);						// CH1のモーターを止める
  LED_R(LOW);
  motorCh2(power);				// CH2のモーターを30％の速さで動かす
  LED_G(HIGH);
  Serial.println("CH2 ");
  delay(5000);						// 5秒待つ
  motorCh2(0);						// CH2のモーターを止める
  LED_G(LOW);
  motorCh3(power);				// CH3のモーターを30％の速さで動かす
  LED_Y(HIGH);
  Serial.println("CH3 ");
  delay(5000);						// 5秒待つ
  motorCh3(0);						// CH3のモーターを止める
  LED_Y(LOW);
  motorCh4(power);				// CH4のモーターを30％の速さで動かす
  LED_B(HIGH);
  Serial.println("CH4 ");
  delay(5000);						// 5秒待つ
  motorCh4(0);						// CH4のモーターを止める
  LED_B(LOW);
  motorStop();				// 全てのモーターを止める
  Serial.println("Stop ");
  delay(1000);						// 1秒待つ
}
