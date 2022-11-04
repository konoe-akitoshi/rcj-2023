/*
  Dribbler Kicker Test program
  for Teensy 3.5
    Jul.31.2020
    Oct.24.2020
    Jan.02.2021
*/

#include "NT_Robot202011.h"
#include "Servo.h"
#include "motorDRV6.h"

void setup() {
  Serial.begin(9600);

  pinMode(Kick_Dir, OUTPUT);
  pinMode(Kicker, OUTPUT);

  // StartSWのLEDpinを初期化する
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
  int i, Start, drbspeed;
  unsigned long ntime;

  if (digitalRead(StartSW) == LOW) {  // StartSW がONなら
    digitalWrite(SWG, LOW);
    digitalWrite(SWR, HIGH);

    Serial.println(" foward");
    dribbler(100);
    delay(2000);
    digitalWrite(Kick_Dir, LOW);
    delay(100);
    digitalWrite(Kicker, HIGH);
    delay(100);

    dribbler(0);
    delay(100);

    digitalWrite(Kicker, LOW);
    delay(1000);
  } else {
    digitalWrite(SWG, HIGH);
    digitalWrite(SWR, LOW);
  }
}
