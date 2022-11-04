/*
  Dribbler Kicker Test program
  for Teensy 3.5
    Jul.31.2020
    Oct.24.2020
    Jan.02.2021
*/

#include "NT_Robot202111.h"
#include "motorDRV5.h"

void setup() {

  Serial.begin(9600);

  pinMode(LINE_LED, OUTPUT);

  //StartSWのLEDpinを初期化する
  pinMode(SWR, OUTPUT);
  pinMode(SWG, OUTPUT);

  digitalWrite(LINE_LED, LOW);
  digitalWrite(SWG, LOW);
  digitalWrite(SWR, LOW);

  Serial.println("Start");
  digitalWrite(SWG, HIGH);
  digitalWrite(SWR, LOW);

}

void loop() {

  if ( digitalRead(StartSW) == LOW) {    // StartSW がONなら
      digitalWrite(SWG, LOW);
      digitalWrite(SWR, HIGH);
      digitalWrite(LINE_LED, HIGH);
    } else {
    digitalWrite(SWG, HIGH);
    digitalWrite(SWR, LOW);
    digitalWrite(LINE_LED, LOW);
  }
}
