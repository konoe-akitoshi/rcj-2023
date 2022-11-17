#include <VL6180X.h>
#include <Wire.h>
#include "NT_Robot202111.h"  // Header file for Teensy 3.5
#include "motorDRV6.h"       // モーター制御のプログラムを読み込む

VL6180X ToF_front;  // create front ToF object

// Low limit voltage 1.1*12 = 13.2
// Mi-NH なら 13.0, Li-po なら 13.5 (Li-po は過放電するので注意！)
const int Vlow = 13.0;

int blob_count;
int openMV[39];

int8_t gyro_o;
int robot_dir, power;
float ball_dir, pre_dir;
float Kp, Kd, Ki;
float data_sum, data_diff;

bool emergency;
bool outofbounds;  // "out of bounds" flag

bool lineflag;  // line
int line[4];

int sig;
float z, m;  // arctan
float x, y;
float bg_x, bg_y;
float yg_x, yg_y;
float goal_x, goal_y;
float y_sig, b_sig, goal_sig;
int ball_front;

float xbee_x, xbee_y;
uint8_t xbee_date;
float p_ball;

float ball_dist, wrap;

float gyro;

bool kick;
unsigned long prev, curr, interval;

void keeper();
void attacker();
int powerLimit(int max, int power);
int get_openMV_coordinate();
int getOpenMV();
void intHandle();
void back_Line1(int power);
void back_Line2(int power);
void back_Line3(int power);
void back_Line4(int power);
float checkvoltage(float Vlow);
void doOutofbound();

void setup() {
    prev = 0;
    interval = 500;  // 待機時間

    // 制御パラメータの設定
    Kp = 0.45;      //  比例要素の感度
    Ki = 0.1;       //  積分要素の感度
    Kd = 0.025;     //  微分要素の感度
    data_diff = 0;  // 前回観測値と今回観測値の差分
    data_sum = 0;   // 誤差(観測値)の累積値
    ball_dir = 0;
    pre_dir = 0;  // 前回観測値

    pinMode(StartSW, INPUT_PULLUP);

    // IOピンのモード設定
    pinMode(PIN_NATIVE_LED, OUTPUT);

    pinMode(PIN_LINE_LED, OUTPUT);
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

    pinMode(Kick1, OUTPUT);
    pinMode(Kick_Dir, OUTPUT);

    pinMode(SWR, OUTPUT);
    pinMode(SWG, OUTPUT);

    pinMode(Aux1, INPUT);
    pinMode(Aux2, INPUT);

    pinMode(GoalSW, INPUT_PULLUP);

    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_Y, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);

    digitalWrite(Kick1, LOW);
    digitalWrite(Kick_Dir, LOW);
    digitalWrite(SWR, HIGH);
    digitalWrite(SWG, HIGH);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(INT_29, INPUT_PULLUP);  // interrupt port set

    Serial.begin(9600);  //  シリアル出力を初期化
    Serial.println("Starting...");

    //*****************************************************************************
    // Set initial value tovariable

    emergency = false;
    outofbounds = false;
    power = 70;  //  set initial motor power
    //*****************************************************************************

    Wire.begin();
    /*
      CE端子をLOWにするとデバイスがリセットされアドレスが初期値に戻るので注意
    */

    delay(10);
    ToF_front.init();
    ToF_front.configureDefault();
    ToF_front.setAddress(TOF_1);  // 好きなアドレスに設定
    ToF_front.setTimeout(100);
    delay(10);

    Serial.println("Initialize 1 ...");

    motorInit();  //  モーター制御の初期化
    dribInit();   //  ドリブラモーターの初期化

    Serial.println("Initialize 2 ...");
    delay(1000);  //  ドリブラ・キッカーの動作チェック
    dribbler1(100);
    dribbler2(100);
    delay(1000);
    dribbler1(0);
    dribbler2(0);
    delay(100);
    digitalWrite(Kick_Dir, LOW);
    delay(100);
    digitalWrite(Kicker, HIGH);
    delay(100);
    digitalWrite(Kicker, LOW);
    delay(1000);
    /*digitalWrite(Kick_Dir, HIGH);
      delay(100);
      digitalWrite(Kicker, HIGH);
      delay(100);
      digitalWrite(Kicker, LOW);
      delay(100);*/

    Serial.println("Initialize 3 ...");

    Serial3.begin(19200);   // initialize serialport for openMV
    Serial2.begin(115200);  // WT901 IMU Sener
    Serial1.begin(9600);    // xbee

    lineflag = false;  // reset outofbounds flag
    for (int i = 0; i < 4; ++i) {
        line[i] = false;
    }
    robot_dir = 0;  // reset robot direction

    // Caution D29 -> Interrupt5

    attachInterrupt(INT_29, intHandle, RISING);

    // LED を初期化する
    // LED_Init();
    digitalWrite(PIN_LED_R, LOW);  // LED_R 消灯
    digitalWrite(PIN_LED_Y, LOW);  // LED_Y 消灯
    digitalWrite(PIN_LED_G, LOW);  // LED_G 消灯
    digitalWrite(PIN_LED_B, LOW);  // LED_B 消灯

    digitalWrite(SWR, HIGH);
    digitalWrite(SWG, HIGH);
    p_ball = 255;
    Serial.println("Initialize end");
}

void loop() {
    digitalWrite(PIN_LED_B, LOW);
    blob_count = get_openMV_coordinate();
    int x_data_ball = (openMV[5] & 0b0000000000111111) + ((openMV[6] & 0b0000000000111111) << 6);
    int y_data_ball = (openMV[7] & 0b0000000000111111) + ((openMV[8] & 0b0000000000111111) << 6);
    // int w_data_ball = (openMV[9] & 0b0000000000111111) + ((openMV[10] & 0b0000000000111111) << 6);
    // int h_data_ball = (openMV[11] & 0b0000000000111111) + ((openMV[12] & 0b0000000000111111) << 6);

    int x_data_yellowgoal = (openMV[18] & 0b0000000000111111) + ((openMV[19] & 0b0000000000111111) << 6);
    int y_data_yellowgoal = (openMV[20] & 0b0000000000111111) + ((openMV[21] & 0b0000000000111111) << 6);
    // int w_data_yellowgoal = (openMV[22] & 0b0000000000111111) + ((openMV[23] & 0b0000000000111111) << 6);
    // int h_data_yellowgoal = (openMV[24] & 0b0000000000111111) + ((openMV[25] & 0b0000000000111111) << 6);

    int x_data_bluegoal = (openMV[31] & 0b0000000000111111) + ((openMV[32] & 0b0000000000111111) << 6);
    int y_data_bluegoal = (openMV[33] & 0b0000000000111111) + ((openMV[34] & 0b0000000000111111) << 6);
    // int w_data_bluegoal = (openMV[35] & 0b0000000000111111) + ((openMV[36] & 0b0000000000111111) << 6);
    // int h_data_bluegoal = (openMV[37] & 0b0000000000111111) + ((openMV[38] & 0b0000000000111111) << 6);

    if (lineflag) {
        lineflag = false;
    }

    // get gyrodata
    if (Serial2.available() > 0) {
        while (Serial2.available() != 0) {  //  Gyro の方位データを gyro に取り込む
            gyro_o = Serial2.read();
        }
    }

    gyro = gyro_o;

    // Xbeeからの信号を読む
    if (Serial1.available() > 0) {
        while (Serial1.available() > 0) {
            p_ball = Serial1.read();
        }
    }
    // openMVのデーターを変換

    sig = openMV[1];  //  openMVのデータを sig, x, y, w, h に取り込む
    x = x_data_ball;
    y = y_data_ball;
    b_sig = openMV[27];
    bg_x = x_data_bluegoal;
    bg_y = y_data_bluegoal;
    y_sig = openMV[14];
    yg_x = x_data_yellowgoal;
    yg_y = y_data_yellowgoal;

    if (sig != 0) {  // 中心補正
        x = 156 - x;
        y = 67 - y;
    }
    if (digitalRead(GoalSW)) {  // 青色ゴールをする場合
        if (y_sig != 0) {
            yg_x = 154 - yg_x;
            yg_y = yg_y - 184;
        }
        if (b_sig != 0) {
            bg_x = 151 - bg_x;
            bg_y = bg_y - 58;
        }
    } else {  // 黄色ゴールをする場合
        if (y_sig != 0) {
            yg_x = 151 - yg_x;
            yg_y = yg_y - 63;
        }
        if (b_sig != 0) {
            bg_x = 156 - bg_x;
            bg_y = bg_y - 184;
        }
    }

    /*if (sig != 0) { //補正
      }
      if (y_sig != 0) {
      yg_x = -(yg_x * 211800) / (140200 + 708 * sqrt(yg_x * yg_x + 28900));
      yg_y = (yg_y * 194600) / (140200 + 708 * sqrt(yg_y * yg_y + 28900));
      }
      if (b_sig != 0) {
      bg_x = -(bg_x * 211800) / (140200 + 708 * sqrt(bg_x * bg_x + 28900));
      bg_y = (bg_y * 194600) / (140200 + 708 * sqrt(bg_y * bg_y + 28900));
      }*/

    if (sig != 0) {  // xbeedate 生成
        xbee_x = x;
        xbee_y = y;
    }
    if (x == 4095) {
        xbee_date = 255;
    } else {
        xbee_date = sqrt(xbee_x * xbee_x + xbee_y * xbee_y);
    }

    ball_dist = sqrt(x * x + y * y);

    Serial1.write(xbee_date);  // xbee へ出力

    Serial.print("ball_x:");
    Serial.print(x);
    Serial.print(" ,ball_y:");
    Serial.print(y);
    Serial.print(" ,tof_front=");
    Serial.println(ball_front);
    Serial.print(" ,yellowgoal_x=");  // 青ゴールのx座標
    Serial.print(bg_x);
    Serial.print(" ,yellowgoal_y=");  // 青ゴールのy座標
    Serial.print(bg_y);
    Serial.print(" ,tan=");
    Serial.println(atan2(bg_x, -bg_y));
    //    Serial.println();

    ball_front = ToF_front.readRangeSingleMillimeters();

    if (digitalRead(StartSW) == LOW) {  // STartSW == Low でスタート
        digitalWrite(SWR, HIGH);
        digitalWrite(SWG, HIGH);

        // checkvoltage(Vlow);  //  電池の電圧をチェック
        if (emergency) {
            Serial.println("");
            Serial.println("  Battery Low!");
            doOutofbound();  //  故障なのでコートの外へ
        }

        checkvoltage(Vlow);
        if (emergency) {          // 電池の電圧が下がっていたら
            digitalWrite(PIN_LINE_LED, LOW);  // ラインセンサのLEDを消灯
            motorFree();                  // モーターを停止
            while (1) {                   // 無限ループ
                digitalWrite(SWR, LOW);
                digitalWrite(SWG, LOW);
                delay(300);
                digitalWrite(SWR, HIGH);
                digitalWrite(SWG, HIGH);
                delay(300);
            }
        }
        digitalWrite(PIN_LINE_LED, HIGH);  // ラインセンサのLEDを点灯
        if (lineflag) {
            lineflag = false;
        }
        // PID

        // 役割判定
        if (digitalRead(Aux1) == LOW) {
            attacker();
        } else if (digitalRead(Aux2) == LOW) {
            keeper();

        } else {
            if (p_ball <= xbee_date) {  // どちらがボールに近いか
                keeper();
            } else {
                attacker();
            }
        }

    } else {  // ロボット停止
        motorFree();
        dribbler1(0);
        dribbler2(0);
        digitalWrite(PIN_LINE_LED, LOW);  // ラインセンサのLEDを消灯
        digitalWrite(SWR, HIGH);
        digitalWrite(SWG, HIGH);
        wrap = 0;
    }
}

void keeper() {
    dribbler1(0);
    wrap = 0;

    if (digitalRead(GoalSW)) {  // 青色の場合
        goal_sig = y_sig;
        goal_x = -yg_x;
        goal_y = yg_y;
    } else {  // 黄色の場合
        goal_sig = b_sig;
        goal_x = -bg_x;
        goal_y = bg_y;
    }

    digitalWrite(LED_BUILTIN, LOW);
    if (ball_dist - p_ball < 60 || sig == 0) {  // ボールとの距離の差が近い、ボールを任せてゴール前に帰る
        if (goal_sig == 0) {
            motorfunction(PI, 100, -gyro);
        } else if (goal_y > 23) {  // ゴールから遠い
            z = atan2(goal_x, goal_y - 23) + 3.14;
            motorfunction(z, 100, -gyro);
        } else if (goal_y < 23 && goal_y > 15 && abs(goal_x) > 33) {  // x座標が０から遠い
            z = atan2(goal_x, goal_y - 23) + 3.14;
            motorfunction(z, 100, -gyro);
        } else if (goal_y < 15) {  // ゴールエリアの横にいるとき
            if (goal_x > 0) {
                motorfunction(-0.60, 60, 0);
            } else {
                motorfunction(0.60, 60, 0);
            }
        } else {  // ゴール前にいるとき
            motorfunction(0, 0, 0);
        }
    } else {  // ボールとの距離の差が遠い、自ら近づく
        float az = atan2(x, sqrt(y));
        motorfunction(az, sqrt(x * x + y * y / 4), -gyro);
    }
}

void attacker() {
    float Pmax = power;
    if (digitalRead(GoalSW)) {  // GoalSWは攻める方向をスイッチに入れる,
        // 相手ゴールの座標は機体中心
        goal_sig = b_sig;
        goal_x = bg_x;
        goal_y = -bg_y;
    } else {
        goal_sig = y_sig;
        goal_x = yg_x;
        goal_y = -yg_y;
    }
    // PID制御をするので
    // 制御値＝誤差(方位)値＋誤差(方位)の時間積分値＋誤差(方位)の時間微分値
    //
    // Convert coordinates data
    if (blob_count != 0) {  // 物体を検出したら
        float fx = 150 - x;       // ロボットが原点に来るようjに座標を変換
        float fy = 130 - y;

        if (fy > 0) {  // 正面から見たボールの方位(radian)を計算
            ball_dir = atan(fx / fy);
        } else if (fx > 0) {
            ball_dir = atan((-fy) / fx) + 1.571;  // 1.571 = Pi/2
        } else {
            ball_dir = atan((-fy) / fx) - 1.571;
        }
        ball_dir = ball_dir + 0.150;  // +0.150は製作誤差による方位のオフセット補正値(radian)
    }
    data_diff = ball_dir - pre_dir;                                       // 前回の方位との差分を計算
    data_sum += data_diff;                                                // 方位誤差の累積を計算
    float Pcontrol = power * (Kp * ball_dir + Ki * data_sum + Kd * data_diff);  // PIDの制御値を計算
    pre_dir = ball_dir;                                                   // 今回の値を代入し次周期から見て前回観測値にする

    digitalWrite(LED_BUILTIN, LOW);
    if (-5 <= y && y <= 30) {  // ボールが前(0 <= y <= 0)にあるとき
        dribbler1(100);
        wrap = 0;
        if (abs(x) < 5) {                // 目の前
            if (ball_front <= 60) {      // y の距離近い
                if (ball_front <= 30) {  // 保持
                    data_sum = 0;
                    // curr = millis(); //始めだけ値をとりたい
                    // if(prev == 0){
                    //     prev = curr;
                    // }
                    // if((curr - prev) >= interval) {
                    // 動作
                    if (goal_sig == 0) {  // ゴールなし
                        motorfunction(0, 80, -gyro);
                    } else if (goal_y <= 33 && abs(goal_x) < 17) {  // ゴールにけれる距離
                        kick = true;
                        digitalWrite(Kick_Dir, LOW);
                        dribbler1(0);
                        digitalWrite(Kicker, HIGH);
                        delay(200);
                        motorfunction(0, 0, 0);
                        delay(800);
                        digitalWrite(Kicker, LOW);
                    } else if (goal_y < 5) {            // ゴールに近づいた時
                        motorfunction(PI, 100, -gyro);  // 後ろに下がる
                    } else {                            // ゴール見えてて近くない
                        z = atan2(goal_x, goal_y);
                        motorfunction(z, powerLimit(Pmax, Pcontrol), -gyro);
                    }
                } else {  // 目の前のボールを保持しに行く
                    kick = false;
                    data_sum = 0;
                    // prev = 0; //ボール保持時間リセット
                    motorfunction(0, 50, -gyro);
                }
            } else {
                z = atan2(x, y);
                motorfunction(z, powerLimit(Pmax, Pcontrol),
                              -gyro);  // ココボール前 制御甘い？
            }
        } else {
            z = atan2(x, y);
            motorfunction(z, powerLimit(Pmax, Pcontrol), -gyro);
        }
    } else if (y <= 0) {  // 後ろにボールがあるとき
        dribbler1(0);
        if (abs(x) < 30) {
            if (y >= -129) {
                motorfunction(0, 50, -gyro);
                wrap = 0;
            } else if (y <= -150) {
                motorfunction(PI, abs(y) / 2.4, -gyro);
                wrap = 0;
            } else if (abs(x) < 5 + abs(y) / 5) {
                if (goal_x > 0 || wrap == 1) {
                    z = atan2(x + 800, y * 3);
                    motorfunction(z, sqrt(x * x + y * y) + 10, -gyro);
                    wrap = 1;
                } else {
                    z = atan2(x - 800, y * 3);
                    motorfunction(z, sqrt(x * x + y * y) + 10, -gyro);
                    wrap = 0;
                }
            } else {
                wrap = 0;
                z = atan2(x, y * 3);
                motorfunction(z, sqrt(x * x + y * y) + 10, -gyro);
            }
        } else {
            wrap = 0;
            z = atan2(x, y * 4);
            motorfunction(z, sqrt(x * x + y * y) + 10, -gyro);
        }
    } else {  // 30 > y になるとき
        dribbler1(0);
        dribbler2(0);
        wrap = 0;
        if (sig == 0) {  // ボールがないとき(y = 4096)
            motorfunction(0, 0, 0);
        } else {                          // ボールがあるとき
            motorfunction(0, 80, -gyro);  // これでたまに回り込みがおおげさになる？
        }
    }
    Serial.print("z");
    Serial.print(z);
    Serial.print(" dir ");
    Serial.print(ball_dir);
    Serial.print(" sum ");
    Serial.print(data_sum);
    Serial.print(" diff ");
    Serial.print(data_diff);
    Serial.print(" Pcontrol ");
    Serial.print(Pcontrol);
    Serial.print(" prev ");
    Serial.print(prev);
    Serial.print(" curr ");
    Serial.print(curr);
    Serial.print(" kick ");
    Serial.println(kick);
}

int powerLimit(int max, int power) {  // powerの値がmax(ex.100)を超えないようにする
    if (power > max) {
        return max;
    } else if (power < -max) {
        return -max;
    }
    return power;
}

int get_openMV_coordinate() {  // get the coordinate data of orange ball
    while (Serial3.available() != 0) {  // buffer flush
        Serial3.read();
    }
    while ((openMV[0] = getOpenMV()) != 254) {
        ;  // wait for "254"
    }
    for (int i = 1; i < 39; ++i) {
        openMV[i] = getOpenMV();
    }
    return openMV[0];
}

int getOpenMV() {  // get serial data from openMV
    while (Serial3.available() == 0) {
        ;  // wait for serial data
    }
    return Serial3.read();
}

//*****************************************************************************
// interrupt handler
// 割り込みの処理プログラム
// Lineを踏んだらバックする

void intHandle() {  // Lineを踏んだらlineflagをセットして止まる。
    digitalWrite(PIN_LED_B, HIGH);

    if (digitalRead(StartSW) == HIGH) {  // スイッチがOFFなら何もしない。
        return;
    }
    int power = 30;

    while (digitalRead(INT_29) == HIGH) {   // Lineセンサが反応している間は繰り返す
        if (digitalRead(LINE1D) == HIGH) {  // lineを踏んだセンサーを調べる
            back_Line1(power);              // Lineセンサと反対方向へ移動する
            lineflag = true;                // set lineflag
        } else if (digitalRead(LINE2D) == HIGH) {
            back_Line2(power);
            lineflag = true;  // set lineflag
        } else if (digitalRead(LINE3D) == HIGH) {
            back_Line3(power);
            lineflag = true;  // set lineflag
        } else if (digitalRead(LINE4D) == HIGH) {
            back_Line4(power);
            lineflag = true;  // set lineflag
        } else {
            digitalWrite(PIN_LED_R, HIGH);
        }
    }

    digitalWrite(PIN_LED_B, LOW);
    digitalWrite(PIN_LED_R, LOW);

    if (lineflag == false) {  // センサーの反応がない場合は何もしない
        return;
    }
    lineflag = true;  // set lineflag
    motorStop();      // ラインから外れたらモーターstop
    return;
}

void back_Line1(int power) {  // Lineセンサ1が反応しなくなるまで後ろに進む
    float azimuth;
    digitalWrite(PIN_LED_R, LOW);  // LED_R点灯
    while ((digitalRead(LINE1D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE3D) == HIGH)) {
        if (digitalRead(LINE4D) == HIGH) {
            azimuth = 3.14159 * 3.0 / 4.0;  // 後ろ方向(1+4)をradianに変換
        } else if (digitalRead(LINE2D) == HIGH) {
            azimuth = 3.14159 * 5.0 / 4.0;  // 後ろ方向(1+2)をradianに変換
        } else {
            azimuth = 3.14159 * 4.0 / 4.0;  // 後ろ方向(3)をradianに変換
        }
        motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
    }
    digitalWrite(PIN_LED_R, LOW);  // LED_R消灯
    motorStop();
}

void back_Line2(int power) {  // Lineセンサ2が反応しなくなるまで左に進む
    float azimuth;
    digitalWrite(PIN_LED_Y, LOW);  // LED_Y点灯
    while ((digitalRead(LINE2D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE4D) == HIGH)) {
        if (digitalRead(LINE1D) == HIGH) {
            azimuth = 3.14159 * 5.0 / 4.0;  // 後ろ方向(2+1)を radian に変換
        } else if (digitalRead(LINE3D) == HIGH) {
            azimuth = 3.14159 * 7.0 / 4.0;  // 後ろ方向(2+3)を radian に変換
        } else {
            azimuth = 3.14159 * 6.0 / 4.0;  // 後ろ方向(4)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
    digitalWrite(PIN_LED_Y, LOW);  // LED_Y 消灯
    motorStop();
}

void back_Line3(int power) {  // Lineセンサ3 が反応しなくなるまで前に進む
    float azimuth;
    digitalWrite(PIN_LED_G, LOW);  // LED_G点灯
    while ((digitalRead(LINE3D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE1D) == HIGH)) {
        if (digitalRead(LINE4D) == HIGH) {
            azimuth = 3.14159 * 1.0 / 4.0;  // 後ろ方向(3+4)を radian に変換
        } else if (digitalRead(LINE2D) == HIGH) {
            azimuth = 3.14159 * 7.0 / 4.0;  // 後ろ方向(3+2)を radian に変換
        } else {
            azimuth = 3.14159 * 0.0 / 4.0;  // 後ろ方向(1)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
    digitalWrite(PIN_LED_G, LOW);  // LED_G 消灯
    motorStop();
}

void back_Line4(int power) {  // Lineセンサ4 が反応しなくなるまで右に進む
    float azimuth;
    digitalWrite(PIN_LED_B, LOW);  // LED_B 点灯
    while ((digitalRead(LINE4D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE2D) == HIGH)) {
        if (digitalRead(LINE3D) == HIGH) {
            azimuth = 3.14159 * 1.0 / 4.0;  // 後ろ方向(4+3)を radian に変換
        } else if (digitalRead(LINE1D) == HIGH) {
            azimuth = 3.14159 * 3.0 / 4.0;  // 後ろ方向(4+1)を radian に変換
        } else {
            azimuth = 3.14159 * 2.0 / 4.0;  // 後ろ方向(2)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
    digitalWrite(PIN_LED_B, LOW);  // LED_B 消灯
    motorStop();
}

// 割り込みの処理プログラム終わり
//*****************************************************************************

//*****************************************************************************
// 電池電圧を監視して電圧が下がったらOutOfBounceさせる処理

float checkvoltage(float Vlow) {  // 電池電圧を監視する。
    int limit = Vlow / 0.01811;
    int voltage = analogRead(Vbatt);  // Get Volatge
    if (voltage < limit) {        // 電圧が Vlow 以下であれば emergency をセットする。
        emergency = true;
        digitalWrite(SWG, HIGH);
        digitalWrite(SWR, HIGH);
    }
    return voltage * 0.01811;
}

void doOutofbound() {  // 強制的に Out of bounds させる。

    detachInterrupt(5);           // Out of bounds するために割込みを禁止する
    digitalWrite(PIN_LINE_LED, LOW);  // ラインセンサの LED を消灯

    while (true) {  // 無限ループ
        if (digitalRead(StartSW) == LOW) {
            motorfunction(3.14159 / 2.0, 30, 0);
        } else {  // スタートスイッチが切られたら止まる
            motorfunction(3.14159 / 2.0, 0, 0);
        }
        digitalWrite(SWG, LOW);
        digitalWrite(SWR, LOW);
        delay(25);
        digitalWrite(SWG, HIGH);
        digitalWrite(SWR, HIGH);
        delay(25);
    }
}
