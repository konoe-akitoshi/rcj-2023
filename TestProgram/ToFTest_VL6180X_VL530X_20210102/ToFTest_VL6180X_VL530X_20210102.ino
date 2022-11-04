/* This minimal example shows how to get single-shot Short range
  measurements from the VL6180X.

  The range readings are in units of mm.
  Max 100mm
*/

#include "NT_Robot202011.h"
#include "motorDRV4.h"

#include <Wire.h>
#include <Servo.h>
#include <PCF8574.h>
#include <VL6180X.h>
#include <VL53L0X.h>

PCF8574 pcf8574(I2C_PCF8574);

VL6180X ToF_front;  // create front ToF object
VL6180X ToF_back;   // create back ToF object

VL53L0X ToF_Long1;   // create LingRange ToF object

const int drbspeed = 50;
const int Vlow = 13.0;  // Low limit voltage 1.1*12 = 13.2
const float Vstep = 0.01811;  // Voltage step 15.3V/845 = 0.01811

void setup()
{
  int i, pin;

  //StartSWのLEDpinを初期化する
  pinMode(SWR, OUTPUT);
  pinMode(SWG, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(Vbatt, INPUT);

  digitalWrite(SWG, LOW);
  digitalWrite(SWR, LOW);
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_B, LOW);

  Serial.begin(9600);
  Wire.begin();

  Serial.println("  Start01 ");


  // Set PCF8574pinMode to OUTPUT
  for ( i = 0; i < 8; i++)
    pcf8574.pinMode(i, OUTPUT);
  pcf8574.begin();
  for ( pin = 0; pin < 8; pin++)
    pcf8574.digitalWrite(pin, HIGH);   //deactivate(reset)ToF_front,ToF_back
  delay(10);
  for ( pin = 0; pin < 8; pin++)
    pcf8574.digitalWrite(pin, LOW);   //deactivate(reset)ToF_front,ToF_back

  /*
    CE端子をLOWにするとデバイスがリセットされアドレスが初期値に
    　戻るので注意
  */

  Serial.println("  Start02 ");

  delay(10);
  pcf8574.digitalWrite(1 - 1, HIGH); //ActivateToF_front VL6180X
  delay(10);
  ToF_front.init();
  ToF_front.configureDefault();
  ToF_front.setAddress(TOF_1);  //好きなアドレスに設定
  ToF_front.setTimeout(10);
  delay(10);

  pcf8574.digitalWrite(2 - 1, HIGH); //Activate ToF_back VL6180X
  delay(10);
  ToF_back.init();
  ToF_back.configureDefault();
  ToF_back.setAddress(TOF_2);  //好きなアドレスに設定
  ToF_back.setTimeout(10);
  delay(10);

  pcf8574.digitalWrite(3 - 1, HIGH); //ActivateToF_front VL6180X
  delay(10);
  ToF_Long1.init();
//  ToF_Long1.configureDefault();
  ToF_Long1.setAddress(TOF_3);  //好きなアドレスに設定
  ToF_Long1.setTimeout(10);
  delay(10);

  Serial.println("  Start03 ");

  //  ドリブラのコントローラ(ESC)を初期化する
  //  dribInit();
  Serial.println("  Initialize end ");
  delay(1000);
}

void loop()
{
  int ball_front, ball_back, ball_Long1;
  float v;

  digitalWrite(SWG, HIGH);
  digitalWrite(SWR, LOW);

  ball_back = ToF_back.readRangeSingleMillimeters();   // 後方ToFの値を読む
  ball_front = ToF_front.readRangeSingleMillimeters(); // 前方ToFの値を読む
  ball_Long1 = ToF_Long1.readRangeSingleMillimeters(); // 長距離ToFの値を読む

  Serial.print(" ToF_front=");
  Serial.print(ball_front);
  if (ToF_front.timeoutOccurred()) {
    Serial.print(" ToF_front TIMEOUT");
    digitalWrite(LED_G, HIGH);
    ball_front = 1000;
  } else {
    digitalWrite(LED_G, LOW);
  }

  Serial.print(" ToF_back=");
  Serial.print(ball_back);
  if (ToF_back.timeoutOccurred()) {
    Serial.print(" ToF_back TIMEOUT");
    digitalWrite(LED_Y, HIGH);
    ball_back = 1000;
  } else {
    digitalWrite(LED_Y, LOW);
  }

  Serial.print(" ToF_Long1=");
  Serial.print(ball_Long1);
  if (ToF_Long1.timeoutOccurred()) {
    Serial.print(" ToF_Long1 TIMEOUT");
    digitalWrite(LED_Y, HIGH);
    ball_back = 1000;
  } else {
    digitalWrite(LED_Y, LOW);
  }

  if (ball_front < 100) {
    digitalWrite(LED_R, HIGH);
  } else {
    digitalWrite(LED_R, LOW);
  }
  if (ball_back < 100) {
    digitalWrite(LED_B, HIGH);
  } else {
    digitalWrite(LED_B, LOW);
  }
  v = analogRead(Vbatt) * Vstep;;
  Serial.print(" Vbatt=");
  Serial.println(v, 4);
  Serial.println();
  delay(100);
}
