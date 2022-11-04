#include "NT_Robot202011.h"

int ledpin = 13;
int i = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(Vbatt, INPUT);
  Serial.begin(9600);
}

void loop() {
  int i;
  // put your main code here, to run repeatedly:
  i = analogRead(Vbatt);
  Serial.print(" Vbatt ");
  Serial.println(i);
  delay(100);

}
