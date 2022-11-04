//
// Line senser test program
//  for Offence Robot
//           Ver 4.0 May.04.2020
//           By Saeki Hidekazu
//           Ver 4.1 Nov.14.2020
//           Ver 4.2 Dec.06.2020
//           Ver 4.3 Dec.23.2020
//           Ver 4.4 Jan.05.2021
//           By Saeki Hidekazu
//		  for Teensy 3.5
//　ロボット本体をランダムな方向に動かしながら、ラインから出ないようにする。
//　ラインを踏んだら、反応したラインセンサと反対方向に移動する。
//　割込みルーチン中でバックする仕様
//  電池電圧が低下するとアウトオブバウンズする機能を追加
//

// Motor control program & LED data

#include "NT_Robot202011.h"
#include "motorDRV4.h"
//#include "LED_TEST2.h"

const float VolLimit = 13.5;  // 限界電圧値(for LiPo)

static int  power = 60;   // 移動速度を保持する
static int  robot_dir;		// 移動方向を保持する
static boolean emergency; // 故障フラグ
static int  lineflag;     // check out of bounds flag

void setup() {
  int i;
  // put your setup code here, to run once:

  // Line Senserの使用ピンを初期化する
  pinMode(LINE_LED, OUTPUT);
  pinMode(LINE1D, INPUT_PULLUP);
  pinMode(LINE2D, INPUT_PULLUP);
  pinMode(LINE3D, INPUT_PULLUP);
  pinMode(LINE4D, INPUT_PULLUP);
  pinMode(LINE5D, INPUT_PULLUP);
  pinMode(LINE1A, INPUT);
  pinMode(LINE2A, INPUT);
  pinMode(LINE3A, INPUT);
  pinMode(LINE4A, INPUT);
  pinMode(LINE5A, INPUT);

  //ゴールSWの使用ピンを初期化する
  pinMode(GoalSW, INPUT);
  pinMode(StartSW, INPUT_PULLUP);

  pinMode(Aux1, INPUT);
  pinMode(Aux2, INPUT);

  //StartSWのLEDpinを初期化する
  pinMode(SWR, OUTPUT);
  pinMode(SWG, OUTPUT);

  // 割り込みピンを初期化する
  pinMode(INT_29, INPUT);          // interrupt set

  // 電池電圧ピンを初期化する
  pinMode(Vbatt, INPUT);  // Voltage read pin

  // 故障フラグをクリアする
  emergency = false;

  // モーターを初期化する
  motorInit();
  // LEDを初期化する
  LED_Init();
  digitalWrite(LED_R, LOW);  // LED_R消灯
  digitalWrite(LED_Y, LOW);  // LED_Y消灯
  digitalWrite(LED_G, LOW);  // LED_G消灯
  digitalWrite(LED_B, LOW);  // LED_B消灯

  digitalWrite(LINE_LED, HIGH); // ラインセンサのLEDを点灯
  lineflag = false;             // reset lineflag
  robot_dir = 0;      //reset robot direction
  randomSeed(0);      // 乱数を初期化する

  Serial2.begin(115200);  // 方位センサのインターフェースを設定
  Serial1.begin(9600);
  Serial1.print("Starting...\n");

  // 最初は０の方向（正面）へpowerで進む
  robot_dir = 0;

  // 割り込みを初期化する。これはSetupの最後で実行
  // pin5の立下りで割り込む
  // Caution D18 -> Interrupt5
  digitalWrite(SWG, LOW);
  digitalWrite(SWR, LOW);
  digitalWrite(LINE_LED, HIGH); // ラインセンサのLEDを点灯
  delay(5000);  // 5sec Delay

  attachInterrupt(INT_29, intHandle, RISING);
  digitalWrite(SWG, HIGH);
  digitalWrite(SWR, LOW);

}

void loop() {

  int i, level, data;
  unsigned int d_time;
  int8_t gyro;
  float azimuth, batt;

  // put your main code here, to run repeatedly:
  // check line and reverse

    batt = checkvoltage(VolLimit);  // 電池の電圧のチェックする
    Serial1.print("  Time = ");
    Serial1.print(millis());

    level = analogRead(LINE1A); // get potential data
    data = digitalRead(LINE1D);
    Serial1.print(" LINE1 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE2A); // get potential data
    data = digitalRead(LINE2D);
    Serial1.print(" LINE2 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE3A); // get potential data
    data = digitalRead(LINE3D);
    Serial1.print(" LINE3 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE4A); // get potential data
    data = digitalRead(LINE4D);
    Serial1.print(" LINE4 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE5A); // get potential data
    data = digitalRead(LINE5D);
    Serial1.print(" LINE5 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");

    data = digitalRead(INT_29);
    Serial1.print(" INT5 ");
    Serial1.print(data);
    Serial1.print(" ");

    if (Serial2.available() > 0)
      while (Serial2.available() != 0) { //Gyroの方位データをgyroに取り込む
        gyro = Serial2.read();
      }
    Serial1.print("  BATT = ");
    Serial1.print(batt, 4);
    Serial1.print("  Gyro=");
    Serial1.print(gyro);
    Serial1.print("  GoalSW=");
    Serial1.print(digitalRead(GoalSW));
    Serial1.print("  Aux1=");
    Serial1.print(digitalRead(Aux1));
    Serial1.print("  Aux2=");
    Serial1.print(digitalRead(Aux2));
    Serial1.println(" ");
    delay(20);

  if ( digitalRead(StartSW) == LOW) {    // StartSW がONなら
    digitalWrite(SWG, LOW);
    digitalWrite(SWR, HIGH);


    if (lineflag == true) {         // lineを踏んでいたら
      robot_dir = random(8);        // 乱数で新しい進行方向をセットする。
      lineflag = false;             // lineflagをクリアする。
      delay(300);
    }
    /*
    if (Serial2.available() > 0)
      while (Serial2.available() != 0) { //Gyroの方位データをgyroに取り込む
        gyro = Serial2.read();
      }
      */
    gyro = -gyro ;  // ロボットが右(gyro > 0)を向いているときはCCWに回転
    azimuth = robot_dir * 3.14159 / 4.0;  // 方位をradianに変換
    motorfunction(azimuth, power, gyro);  // robot_dirの方向に進ませる

    batt = checkvoltage(VolLimit);  // 電池の電圧のチェックする
    Serial1.print("  Time = ");
    Serial1.print(millis());
    Serial1.print("  BATT = ");
    Serial1.print(batt, 4);
    Serial1.print("  Gyro=");
    Serial1.println(gyro);
    delay(20);

    if ( emergency == true ) {
      Serial1.println("");
      Serial1.println("  Battery Low!");
      doOutofbound();    //  故障なのでコートの外へ
    }
  }
  else {
    motorFree(); //  停止（スイッチが切られた）
    digitalWrite(SWG, HIGH);
    digitalWrite(SWR, LOW);
    /*
    batt = checkvoltage(VolLimit);  // 電池の電圧のチェックする
    Serial1.print("  Time = ");
    Serial1.print(millis());

    level = analogRead(LINE1A); // get potential data
    data = digitalRead(LINE1D);
    Serial1.print(" LINE1 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE2A); // get potential data
    data = digitalRead(LINE2D);
    Serial1.print(" LINE2 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE3A); // get potential data
    data = digitalRead(LINE3D);
    Serial1.print(" LINE3 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE4A); // get potential data
    data = digitalRead(LINE4D);
    Serial1.print(" LINE4 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");
    level = analogRead(LINE5A); // get potential data
    data = digitalRead(LINE5D);
    Serial1.print(" LINE5 ");
    Serial1.print(level);
    Serial1.print(" ");
    Serial1.print(data);
    Serial1.print(" ");

    data = digitalRead(INT_29);
    Serial1.print(" INT5 ");
    Serial1.print(data);
    Serial1.print(" ");

    if (Serial2.available() > 0)
      while (Serial2.available() != 0) { //Gyroの方位データをgyroに取り込む
        gyro = Serial2.read();
      }
    Serial1.print("  BATT = ");
    Serial1.print(batt, 4);
    Serial1.print("  Gyro=");
    Serial1.print(gyro);
    Serial1.print("  GoalSW=");
    Serial1.print(digitalRead(GoalSW));
    Serial1.print("  Aux1=");
    Serial1.print(digitalRead(Aux1));
    Serial1.print("  Aux2=");
    Serial1.print(digitalRead(Aux2));
    Serial1.println(" ");
    */
    delay(100);
  }
}

// interrupt handler
// 割り込みの処理プログラム
// Lineを踏んだらバックする

void intHandle() {  // Lineを踏んだらlineflagをセットして止まる。
  int i, power, back_dir;
  float azimuth;

  if ( digitalRead(StartSW) == HIGH)  // スイッチがOFFなら何もしない。
    return;
  power = 30;

  while (digitalRead(INT_29) == HIGH) {     // Lineセンサが反応している間は繰り返す
    if (digitalRead(LINE1D) == HIGH) {     // lineを踏んだセンサーを調べる
      back_Line1(power);        // Lineセンサと反対方向へ移動する
      lineflag = true;          // set lineflag
    } else if (digitalRead(LINE2D) == HIGH) {
      back_Line2(power);
      lineflag = true;          // set lineflag
    } else if (digitalRead(LINE3D) == HIGH) {
      back_Line3(power);
      lineflag = true;          // set lineflag
    } else if (digitalRead(LINE4D) == HIGH) {
      back_Line4(power);
      lineflag = true;          // set lineflag
    }
  }

  if (lineflag = false)     // センサーの反応がない場合は何もしない
    return;
  lineflag = true;          // set lineflag
  motorStop();                  // ラインから外れたらモーターstop
  return;
}

void back_Line1(int power) {             // Lineセンサ1が反応しなくなるまで後ろに進む
  float azimuth;
  digitalWrite(LED_R, HIGH);  // LED_R点灯
  while ((digitalRead(LINE1D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE3D) == HIGH)) {
    if (digitalRead(LINE4D) == HIGH) {
      azimuth = 3.14159 * 3.0 / 4.0;   // 後ろ方向(1+4)をradianに変換
    } else if (digitalRead(LINE2D) == HIGH) {
      azimuth = 3.14159 * 5.0 / 4.0;   // 後ろ方向(1+2)をradianに変換
    } else {
      azimuth = 3.14159 * 4.0 / 4.0;   // 後ろ方向(3)をradianに変換
    }
    motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
  }
  digitalWrite(LED_R, LOW);  // LED_R消灯
  motorStop();
}

void back_Line2(int power) {             // Lineセンサ2が反応しなくなるまで左に進む
  float azimuth;
  digitalWrite(LED_Y, HIGH);  // LED_Y点灯
  while ((digitalRead(LINE2D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE4D) == HIGH)) {
    if (digitalRead(LINE1D) == HIGH) {
      azimuth = 3.14159 * 5.0 / 4.0;   // 後ろ方向(2+1)をradianに変換
    } else if (digitalRead(LINE3D) == HIGH) {
      azimuth = 3.14159 * 7.0 / 4.0;   // 後ろ方向(2+3)をradianに変換
    } else {
      azimuth = 3.14159 * 6.0 / 4.0;   // 後ろ方向(4)をradianに変換
    }
    motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
  }
  digitalWrite(LED_Y, LOW);  // LED_Y消灯
  motorStop();
}

void back_Line3(int power) {             // Lineセンサ3が反応しなくなるまで前に進む
  float azimuth;
  digitalWrite(LED_G, HIGH);  // LED_G点灯
  while ((digitalRead(LINE3D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE1D) == HIGH)) {
    if (digitalRead(LINE4D) == HIGH) {
      azimuth = 3.14159 * 1.0 / 4.0;   // 後ろ方向(3+4)をradianに変換
    } else if (digitalRead(LINE2D) == HIGH) {
      azimuth = 3.14159 * 7.0 / 4.0;   // 後ろ方向(3+2)をradianに変換
    } else {
      azimuth = 3.14159 * 0.0 / 4.0;   // 後ろ方向(1)をradianに変換
    }
    motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
  }
  digitalWrite(LED_G, LOW);  // LED_G消灯
  motorStop();
}

void back_Line4(int power) {             // Lineセンサ4が反応しなくなるまで右に進む
  float azimuth;
  digitalWrite(LED_B, HIGH);  // LED_B点灯
  while ((digitalRead(LINE4D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE2D) == HIGH)) {
    if (digitalRead(LINE3D) == HIGH) {
      azimuth = 3.14159 * 1.0 / 4.0;   // 後ろ方向(4+3)をradianに変換
    } else if (digitalRead(LINE1D) == HIGH) {
      azimuth = 3.14159 * 3.0 / 4.0;   // 後ろ方向(4+1)をradianに変換
    } else {
      azimuth = 3.14159 * 2.0 / 4.0;   // 後ろ方向(2)をradianに変換
    }
    motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
  }
  digitalWrite(LED_B, LOW);  // LED_B消灯
  motorStop();
}

float checkvoltage(float Vlow) {  // 電池電圧を監視する。
  int voltage, limit;
  limit = Vlow / 0.01811;
  voltage = analogRead(Vbatt); // Get Volatge
  if ( voltage < limit ) {      // 電圧が　Vlow以下であればemergencyをセットする。
    emergency = true;
    digitalWrite(SWG, LOW);
    digitalWrite(SWR, LOW);

  }
  return voltage * 0.01811;
}

void doOutofbound() { // 強制的にOut of bounds させる。
  int pixel;
  uint32_t color;

  detachInterrupt(5);   // Out of bounds するために割込みを禁止する
  digitalWrite(LINE_LED, LOW); // ラインセンサのLEDを消灯

  while (true) {  // 無限ループ
    if ( digitalRead(StartSW) == LOW)
      motorfunction(3.14159 / 2.0, 30, 0);
    else          // スタートスイッチが切られたら止まる
      motorfunction(3.14159 / 2.0, 0, 0);
    digitalWrite(SWG, LOW);
    digitalWrite(SWR, LOW);
    delay(25);
    digitalWrite(SWG, HIGH);
    digitalWrite(SWR, HIGH);
    delay(25);

  }
}

void LED_Init() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
}
