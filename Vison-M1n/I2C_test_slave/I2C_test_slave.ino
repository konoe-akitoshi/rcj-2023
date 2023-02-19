/**********************************************
  Arduino Nano I2C(Slave)
**********************************************/
#include <Wire.h>

byte b = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("/-----START Arduino Nano-----/");
  Serial.println("/----------I2C Slave---------/");

  Wire.begin(0x24);  // Slave ID #21
  Wire.onRequest(requestEvent);
}

void loop() {}

void requestEvent() { Wire.write(b++); }