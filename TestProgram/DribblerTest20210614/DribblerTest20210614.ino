/*
  Dribbler Kicker Test program
  for Teensy 3.5
    Jul.31.2020
    Oct.24.2020
    Jan.02.2021
*/

#include "Servo.h"
#include "NT_Robot202011.h"
#include "motorDRV4.h"

void setup() {

  Serial.begin(9600);

  pinMode(Kick_Dir, OUTPUT);
  pinMode(Kicker, OUTPUT);

  //StartSWのLEDpinを初期化する
  pinMode(SWR, OUTPUT);
  pinMode(SWG, OUTPUT);

  digitalWrite(Kicker, LOW);
  digitalWrite(SWG, LOW);
  digitalWrite(SWR, LOW);

  dribInit();

  Serial.println("Start");
  digitalWrite(SWG, HIGH);
  digitalWrite(SWR, LOW);

}

void loop() {
  int i, Start , drbspeed;
  unsigned long ntime;

  if ( digitalRead(StartSW) == LOW) {    // StartSW がONなら
    digitalWrite(SWG, LOW);
    digitalWrite(SWR, HIGH);

    Serial.println(" foward");
    drbspeed = 1000;
    for ( i = 0; i < 5; i++) {
      dribbler1(i * 10);
      dribbler2(i * 10);
      Serial.println(i * 10);
      delay(1000);
    }
    dribbler1(0);                   // Dribbler Stop
    dribbler2(0);
    Serial.println(" Stop");
    delay(300);
/*
    digitalWrite(Kick_Dir, LOW);    // Set Front Kicker
    delay(20);
    digitalWrite(Kicker, HIGH);     //  キック
    delay(100);
    digitalWrite(Kicker, LOW);
    delay(3000);                    // Wait for Capaciter Charge Up
    digitalWrite(Kick_Dir, HIGH);   // Set Back Kicker
    delay(50);
    digitalWrite(Kicker, HIGH);     //  キック
    delay(100);
    digitalWrite(Kicker, LOW);
*/
  } else {
    digitalWrite(SWG, HIGH);
    digitalWrite(SWR, LOW);
  }
}
