//
// 4ch Motor Board control program
//           Ver 1.0 Jun.24.2018
//           By Saeki Hidekazu
//           23.Oct.2022 By H.Saeki
//           CPU 8MHz
//

#include <Wire.h>

const int I2Caddress = 0x0a;
const int CH1input1 = 12;
const int CH1input2 = 13;
const int CH2input1 =  2;
const int CH2input2 =  4;
const int CH3input1 =  7;
const int CH3input2 =  8;
const int CH4input1 = A0;
const int CH4input2 = A1;

const int CH1enable = 11;
const int CH2enable =  3;
const int CH3enable =  9;
const int CH4enable = 10;

const int STBY      = 5;
const int HEARTBEAT = 6;

static byte motor[6];

void setup() {
  // put your setup code here, to run once:
  pinMode(CH1input1, OUTPUT);
  pinMode(CH1input2, OUTPUT);
  pinMode(CH2input1, OUTPUT);
  pinMode(CH2input2, OUTPUT);
  pinMode(CH3input1, OUTPUT);
  pinMode(CH3input2, OUTPUT);
  pinMode(CH4input1, OUTPUT);
  pinMode(CH4input2, OUTPUT);

  pinMode(STBY, OUTPUT);
  pinMode(HEARTBEAT, OUTPUT);

  digitalWrite(STBY, LOW);  // Reset STBY

  TCCR1B &= B11111000;
  TCCR1B |= B00000001; //set PWM 15.5kHz
  TCCR2B &= B11111000;
  TCCR2B |= B00000001; //set PWM 15.1kHz

  motorCh1(0);  // Set All motors STOP
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
  
  Wire.begin(I2Caddress);
  Wire.onReceive(getdata);
}

void motorCh1(int data) {
  int power;
  if ( data == B10000000 ) {     // if Brake
    analogWrite(CH1enable, 0);  // power = 0
    digitalWrite(CH1input1, HIGH); // Brake
    digitalWrite(CH1input2, HIGH);
    return;
  }
  if ((data & B10000000) == 0) {
    digitalWrite(CH1input1, HIGH); // Forward
    digitalWrite(CH1input2, LOW);
  } else {
    digitalWrite(CH1input1, LOW); // Reverse
    digitalWrite(CH1input2, HIGH);
  }
  power = data & B01111111;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH1enable, power); // 0 < power < 255
}

void motorCh2(int data) {
  int power;
  if ( data == B10000000 ) {     // if Brake
    analogWrite(CH2enable, 0);  // power = 0
    digitalWrite(CH2input1, HIGH); // Brake
    digitalWrite(CH2input2, HIGH);
    return;
  }
  if ((data & B10000000) == 0) {
    digitalWrite(CH2input1, HIGH); // Forward
    digitalWrite(CH2input2, LOW);
  } else {
    digitalWrite(CH2input1, LOW); // Reverse
    digitalWrite(CH2input2, HIGH);
  }
  power = data & B01111111;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH2enable, power); // 0 < power < 255
}

void motorCh3(int data) {
  int power;
  if ( data == B10000000 ) {     // if Brake
    analogWrite(CH3enable, 0);  // power = 0
    digitalWrite(CH3input1, HIGH); // Brake
    digitalWrite(CH3input2, HIGH);
    return;
  }
  if ((data & B10000000) == 0) {
    digitalWrite(CH3input1, HIGH); // Forward
    digitalWrite(CH3input2, LOW);
  } else {
    digitalWrite(CH3input1, LOW); // Reverse
    digitalWrite(CH3input2, HIGH);
  }
  power = data & B01111111;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH3enable, power); // 0 < power < 255
}

void motorCh4(int data) {
  int power;
  if ( data == B10000000 ) {     // if Brake
    analogWrite(CH4enable, 0);  // power = 0
    digitalWrite(CH4input1, HIGH); // Brake
    digitalWrite(CH4input2, HIGH);
    return;
  }
  if ((data & B10000000) == 0) {
    digitalWrite(CH4input1, HIGH); // Forward
    digitalWrite(CH4input2, LOW);
  } else {
    digitalWrite(CH4input1, LOW); // Reverse
    digitalWrite(CH4input2, HIGH);
  }
  power = data & B01111111;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH4enable, power); // 0 < power < 255
}

void getdata(int x) {           // recieve I2C data
  int i;
  if (Wire.available() > 0) {
    for (i = 0; i < x; i++) {
      motor[i] = Wire.read();
    };
  };
  motorCh1(motor[0]);
  motorCh2(motor[1]);
  motorCh3(motor[2]);
  motorCh4(motor[3]);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(HEARTBEAT,HIGH);    // Hart beat
  delay(500);
  digitalWrite(HEARTBEAT,LOW);
  delay(1000);
}
