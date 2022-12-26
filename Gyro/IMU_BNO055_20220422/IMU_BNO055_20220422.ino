//
// BNO055 COntrol program
// for atmega328
// Aug.12.2021 Ver.1.0
// Apr.22.2022 Ver.2.0
// By H.Saeki
// Compatible with Kemarin-Tech GY-M02
//
// Improtant!!
// Fuse bit L set to FFh
//

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* This driver reads raw data from the BNO055

   Connections
   ===========
   Connect VDD to 5V DC
   Connect GROUND to common ground
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect ADO to GROUND
   Connect INT to N.C.
   Connect BOOT to N.C.
   Connect RST to D3

   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (50)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                    ADO=LOW  > Address = 0x28
//                                    ADO=Open > Address = 0x29
//                                   id, address

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

uint8_t tsystem, tgyro, taccel, tmag;
int8_t angle_i, offset_i;

const int tcal = A2;
const int tset = A3;
const int IMUreset = 3;
const int led_r = 4;
const int led_l = 6;
const int led_c = 5;
const int Active = 7;
const int Zset = 8;

const int bright_a = 9;
const int bright_b = 10;

int val_cal, val_set;

void setup()
{
  offset_i = 0;

  //Wire.begin();
  Serial.begin(115200);
  //Serial.println("BNO055 Yaw Output 20210813");
  pinMode(tcal, INPUT_PULLUP);
  pinMode(tset, INPUT_PULLUP);
  pinMode(Zset, INPUT_PULLUP);
  digitalWrite(IMUreset, LOW);
  pinMode(IMUreset, OUTPUT);  // RESET IMU
  pinMode(led_l, OUTPUT);
  pinMode(led_c, OUTPUT);
  pinMode(led_r, OUTPUT);
  pinMode(Active, OUTPUT);
  pinMode(bright_a, OUTPUT);
  pinMode(bright_b, OUTPUT);

  analogWrite(bright_a, 10); // white LED
  analogWrite(bright_b, 90); // green LED

  digitalWrite(led_l, HIGH);
  digitalWrite(led_c, HIGH);
  digitalWrite(led_r, HIGH);
  digitalWrite(Active, HIGH);
  delay(100);
  digitalWrite(IMUreset, HIGH);
  delay(1000);

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1) {
      digitalWrite(led_r, LOW);
      digitalWrite(led_c, LOW);
      digitalWrite(led_l, LOW);
      delay(100);
      digitalWrite(led_r, HIGH);
      digitalWrite(led_c, HIGH);
      digitalWrite(led_l, HIGH);
      delay(100);
    }
  }

  offset_i = 0;
  delay(1000);
  bno.setExtCrystalUse(true);
  //  digitalWrite(Active, HIGH);

}

void loop()
{
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2

  /* Display the floating point data */
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  if (euler.x() < 180.0) {
    angle_i = (euler.x() / 180.0) * 128.0;
  } else {
    angle_i = ((euler.x() - 360.0) / 180.0) * 128.0;
  }
  if ((digitalRead(tset) == LOW) || (digitalRead(Zset) == LOW) ) { // offset値を得る
    digitalWrite(led_r, LOW);
    digitalWrite(led_c, LOW);
    digitalWrite(led_l, LOW);
    offset_i = angle_i;
    delay(300);
    digitalWrite(led_r, HIGH);
    digitalWrite(led_c, HIGH);
    digitalWrite(led_l, HIGH);
  }

  angle_i = angle_i - offset_i;	// offsetを補正

  if (abs(angle_i) < 10) {
    digitalWrite(led_r, HIGH);
    digitalWrite(led_c, LOW);
    digitalWrite(led_l, HIGH);
  }
  if (10 < angle_i) {
    digitalWrite(led_r, LOW);
    digitalWrite(led_c, HIGH);
    digitalWrite(led_l, HIGH);
  }
  if (angle_i < -10) {
    digitalWrite(led_r, HIGH);
    digitalWrite(led_c, HIGH);
    digitalWrite(led_l, LOW);
  }

  Serial.write(angle_i);   // -128～+127の値を出力する
  delay(10);

  /* Check calibration status for each sensor. */
  bno.getCalibration(&tsystem, &tgyro, &taccel, &tmag);
  if (tmag == 3)
    digitalWrite(Active, LOW);
  else
    digitalWrite(Active, HIGH);

  val_cal = digitalRead(tcal);
  if (val_cal == LOW) {        // キャリブレーションモード選択
    digitalWrite(led_r, LOW);
    digitalWrite(led_c, LOW);
    digitalWrite(led_l, LOW);
    //    digitalWrite(IMUreset, LOW); // LOW -> Reset
    delay(1000);
    //    digitalWrite(IMUreset, HIGH); // LOW -> Reset
    digitalWrite(led_r, HIGH);
    digitalWrite(led_c, HIGH);
    digitalWrite(led_l, HIGH);
  }

}
