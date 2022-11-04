//
// FourWheel Motor control module
//           Ver 4.1 Oct.24.2020
//           By Saeki Akitoshi
//           By Saeki Hidekazu
// for Teensy 3.5
//モーターを動かすサブプログラム
//

#include "Servo.h"

//  IOピンを設定する

const int CH1DIR1   = 43;
const int CH1DIR2   = 44;
const int CH2DIR1   = 45;
const int CH2DIR2   = 46;
const int CH3DIR1   = 47;
const int CH3DIR2   = 48;
const int CH4DIR1   = 49;
const int CH4DIR2   = 50;

const int CH1PWM    = 35;
const int CH2PWM    = 36;
const int CH3PWM    = 37;
const int CH4PWM    = 38;

const int DR1PWM = 31;
const int DR2PWM = 32;

Servo myservo1, myservo2; // create servo object to control a servo

void motorCh1(int data) {   //CH1のモーターを動かすプログラム
  int power;
  if ( data == 0 ) {         // data == 0　なら停止
    analogWrite(CH1PWM, 0);  // power = 0
    digitalWrite(CH1DIR1, HIGH); // Brake
    digitalWrite(CH1DIR2, HIGH);
    return;
  }
  if (data > 0) {                //  data > 0 なら正転、data < 0 なら逆転
    digitalWrite(CH1DIR1, HIGH); // Forward
    digitalWrite(CH1DIR2, LOW);
  } else {
    digitalWrite(CH1DIR1, LOW);  // Reverse
    digitalWrite(CH1DIR2, HIGH);
  }
  power = abs(data);            //  -100～+100のデータを0～250に変換する
  if (power > 100)              //  data > 100 の場合は100にする
    power = 100;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH1PWM, power); // 0 < power < 250
}

void motorCh2(int data) {   //CH2のモーターを動かすプログラム
  int power;
  if ( data == 0 ) {     // if Brake
    analogWrite(CH2PWM, 0);  // power = 0
    digitalWrite(CH2DIR1, HIGH); // Brake
    digitalWrite(CH2DIR2, HIGH);
    return;
  }
  if (data > 0) {
    digitalWrite(CH2DIR1, HIGH); // Forward
    digitalWrite(CH2DIR2, LOW);
  } else {
    digitalWrite(CH2DIR1, LOW); // Reverse
    digitalWrite(CH2DIR2, HIGH);
  }
  power = abs(data);
  if (power > 100)
    power = 100;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH2PWM, power); // 0 < power < 250
}

void motorCh3(int data) {   //CH3のモーターを動かすプログラム
  int power;
  if ( data == 0 ) {     // if Brake
    analogWrite(CH3PWM, 0);  // power = 0
    digitalWrite(CH3DIR1, HIGH); // Brake
    digitalWrite(CH3DIR2, HIGH);
    return;
  }
  if (data > 0) {
    digitalWrite(CH3DIR1, HIGH); // Forward
    digitalWrite(CH3DIR2, LOW);
  } else {
    digitalWrite(CH3DIR1, LOW); // Reverse
    digitalWrite(CH3DIR2, HIGH);
  }
  power = abs(data);
  if (power > 100)
    power = 100;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH3PWM, power); // 0 < power < 250
}

void motorCh4(int data) {   //CH4のモーターを動かすプログラム
  int power;
  if ( data == 0 ) {     // if Brake
    analogWrite(CH4PWM, 0);  // power = 0
    digitalWrite(CH4DIR1, HIGH); // Brake
    digitalWrite(CH4DIR2, HIGH);
    return;
  }
  if (data > 0) {
    digitalWrite(CH4DIR1, HIGH); // Forward
    digitalWrite(CH4DIR2, LOW);
  } else {
    digitalWrite(CH4DIR1, LOW); // Reverse
    digitalWrite(CH4DIR2, HIGH);
  }
  power = abs(data);
  if (power > 100)
    power = 100;
  power = (power << 1) + (power >> 1); // power=power*2.5
  analogWrite(CH4PWM, power); // 0 < power < 250
}


void motorInit() {    // Arduinoのモーター制御を初期化する
  pinMode(CH1DIR1, OUTPUT);
  pinMode(CH1DIR2, OUTPUT);
  pinMode(CH2DIR1, OUTPUT);
  pinMode(CH2DIR2, OUTPUT);
  pinMode(CH3DIR1, OUTPUT);
  pinMode(CH3DIR2, OUTPUT);
  pinMode(CH4DIR1, OUTPUT);
  pinMode(CH4DIR2, OUTPUT);

  pinMode(CH1PWM, OUTPUT);
  pinMode(CH2PWM, OUTPUT);
  pinMode(CH3PWM, OUTPUT);
  pinMode(CH4PWM, OUTPUT);

  analogWriteFrequency(CH1PWM, 37000);
  analogWriteFrequency(CH2PWM, 37000);
  analogWriteFrequency(CH3PWM, 37000);
  analogWriteFrequency(CH4PWM, 37000);

  motorCh1(0);  // Set All motors STOP
  motorCh2(0);
  motorCh3(0);
  motorCh4(0);
}

void dribInit() { // ドリブラモーターのESCを初期化する
  
  pinMode(DR1PWM, OUTPUT);
  pinMode(DR2PWM, OUTPUT);

  myservo1.attach(DR1PWM, 1000, 2000); // attaches the servo on pin D7 to the servo object
  //　1000us < Pulse < 2000us
  myservo2.attach(DR2PWM, 1000, 2000); // attaches the servo on pin D7 to the servo object
  //　1000us < Pulse < 2000us
  myservo1.writeMicroseconds(2000);    // To reset ESC, max_value 3sec.
  myservo2.writeMicroseconds(2000);
  delay(2000);
  myservo1.writeMicroseconds(1000);    // To reset ESC, min_value 3sec.
  myservo2.writeMicroseconds(1000);
  delay(2000);
}

// motor control function
// z:azimuth(radian)
// p:power(-100:100)
// rotation :rotation(-100:100)
//

void motorfunction(float z, int power, int rotation) {
  float x[4], x_max, w;
  int i;

  if (power > 100)    // powerを±100以下にする
    power = 100;
  else if (power < -100)
    power = -100;
  if (rotation > 100)    // rotationを±100以下にする
    rotation = 100;
  else if (rotation < -100)
    rotation = -100;

  x[0] = -(sin(z - 3.14159 / 4.0));
  x[1] = -(sin(z - 3 * 3.14159 / 4.0));
  x[2] = -(sin(z - 5 * 3.14159 / 4.0));
  x[3] = -(sin(z - 7 * 3.14159 / 4.0));

  x_max = 0.0;
  for ( i = 0; i < 4; i++) {
    if ( x_max < abs(x[i]))
      x_max = abs(x[i]);
  };
  for ( i = 0; i < 4; i++)
    x[i] = x[i] / x_max;
  w = -(rotation / 100.0);
  for ( i = 0; i < 4; i++)
    x[i] = x[i] + w;
  x_max = 0.0;
  for ( i = 0; i < 4; i++) {
    if ( x_max < abs(x[i]))
      x_max = abs(x[i]);
  };
  for ( i = 0; i < 4; i++)
    x[i] = x[i] / x_max;
  for ( i = 0; i < 4; i++)
    x[i] = x[i] * power;
  /*
    Serial.print(" Z=");
    Serial.print(z);
    Serial.print(" x[0]=");
    Serial.print(int(x[0]));
    Serial.print(" x[1]=");
    Serial.print(int(x[1]));
    Serial.print(" x[2]=");
    Serial.print(int(x[2]));
    Serial.print(" x[3]=");
    Serial.print(int(x[3]));
  */
  motorCh1(x[0]);
  motorCh2(x[1]);
  motorCh3(x[2]);
  motorCh4(x[3]);
}

//  時計回りに回転する

void turnCW(int power) {
  motorCh1(-power); motorCh2(-power); motorCh3(-power); motorCh4(-power);
}

//  反時計回りに回転する

void turnCCW(int power) {
  motorCh1(power); motorCh2(power); motorCh3(power); motorCh4(power);
}

//  モーターを止める(Brake)

void motorStop() {
  motorCh1(0); motorCh2(0); motorCh3(0); motorCh4(0);
}

//  モーターを止める(Free)

void motorFree() {

  digitalWrite(CH1PWM, HIGH);  // power = 0
  digitalWrite(CH1DIR1, LOW);  // Off
  digitalWrite(CH1DIR2, LOW);

  digitalWrite(CH2PWM, HIGH);  // power = 0
  digitalWrite(CH2DIR1, LOW);  // Off
  digitalWrite(CH2DIR2, LOW);

  digitalWrite(CH3PWM, HIGH);  // power = 0
  digitalWrite(CH3DIR1, LOW);  // Off
  digitalWrite(CH3DIR2, LOW);

  digitalWrite(CH4PWM, HIGH);  // power = 0
  digitalWrite(CH4DIR1, LOW);  // Off
  digitalWrite(CH4DIR2, LOW);

}

void dribbler1(int power) { // ドリブラ１を回す. 0 < power <100
  if ( power < 0 || power > 100)
    return;
  myservo1.writeMicroseconds(1000 + power * 10);
  return;
}

void dribbler2(int power) { // ドリブラ２を回す. 0 < power <100
  if ( power < 0 || power > 100)
    return;
  myservo2.writeMicroseconds(1000 + power * 10);
  return;
}

void dribbler(int power) {
  dribbler1(power);
  return;
}
