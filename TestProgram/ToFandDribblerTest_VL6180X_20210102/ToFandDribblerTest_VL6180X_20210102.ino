#include <PCF8574.h>

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

PCF8574 pcf8574(I2C_PCF8574);
VL6180X ToF_front;  // create front ToF object
VL6180X ToF_back;   // create back ToF object

const int drbspeed = 50;

void setup()
{
  int i, pin;

  //StartSWのLEDpinを初期化する
  pinMode(SWR, OUTPUT);
  pinMode(SWG, OUTPUT);

  digitalWrite(SWG, LOW);
  digitalWrite(SWR, LOW);

  Serial.begin(9600);
  Wire.begin();

  Serial.println("  Start01 ");

  // Set PCF8574pinMode to OUTPUT
  for ( i = 0; i < 8; i++)
    pcf8574.pinMode(i, OUTPUT);
  pcf8574.begin();
  for ( pin = 0; pin < 8; pin++)
    pcf8574.digitalWrite(pin, LOW);   //deactivate(reset) ToF_front,ToF_back

  /*
    CE端子をLOWにするとデバイスがリセットされアドレスが初期値に
    　戻るので注意
  */

  Serial.println("  Start02 ");

  delay(10);
  pcf8574.digitalWrite(1 - 1, HIGH); //Activate ToF_front VL6180X
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

  Serial.println("  Start03 ");

  //  ドリブラのコントローラ(ESC)を初期化する
  dribInit();
  Serial.println("  Initialize end ");
  delay(1000);
}

void loop()
{
  int ball_front, ball_back;

  digitalWrite(SWG, LOW);
  digitalWrite(SWR, HIGH);

  Serial.print(" ToF_back=");
  ball_back = ToF_back.readRangeSingleMillimeters();   // 後方ToFの値を読む
  Serial.print(ball_back);
  if (ToF_back.timeoutOccurred()) {
    Serial.print(" ToF_back TIMEOUT");
    ball_back = 1000;
  }
  Serial.print(" ToF_front=");
  ball_front = ToF_front.readRangeSingleMillimeters();  // 前方ToFの値を読む
  Serial.print(ball_front);
  if (ToF_front.timeoutOccurred()) {
    Serial.print(" ToF_front TIMEOUT");
    ball_front = 1000;
  }

  Serial.println();

  if ( ball_front < 50) { // 前方ToFの値が50mm以下なら前方ドリブラを回す
    dribbler1(drbspeed);
  } else {
    dribbler1(0); // 前方ToFの値が50mm以上なら前方ドリブラを止める
  }
  if ( ball_back < 50) {  // 後方ToFの値が50mm以下なら後方ドリブラを回す
    dribbler2(drbspeed);
  } else {
    dribbler2(0);  // 後方ToFの値が50mm以上なら後方ドリブラを止める
  }

}
