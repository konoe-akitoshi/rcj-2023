// デバッグ: 1
// リリース: 0
#define DEBUG_MODE 1

#include <VL6180X.h>
#include <Wire.h>
#include "NT_Robot202111.h"
#include "components/battery.hpp"
#include "components/led_light.hpp"
#include "components/open_mv.hpp"
#include "components/xbee.hpp"
#include "motorDRV6.h"
#include "types/vector2.hpp"

VL6180X ToF_front;  // create front ToF object

int blob_count;

int gyro_o;

bool lineflag = false;

Vector2 ball_pos;
Vector2 blue_goal;
Vector2 yellow_goal;
bool exist_ball;
bool exist_yellow_goal;
bool exist_blue_goal;
int ball_front;

float p_ball = 255;
float ball_dist;
float wrap;

void keeper(const int rotation);
void attacker(const int rotation);
int powerLimit(const int max, const int power);
void intHandle();
void back_Line1(const int power);
void back_Line2(const int power);
void back_Line3(const int power);
void back_Line4(const int power);
float checkvoltage(const float Vlow);
void doOutofbound();

// 制御パラメータの設定
constexpr float Kp = 0.45;   //  比例要素の感度
constexpr float Ki = 0.1;    //  積分要素の感度
constexpr float Kd = 0.025;  //  微分要素の感度

constexpr int Power = 70;  // initial motor power

// Low limit voltage 1.1*12 = 13.2
// Mi-NH なら 13.0, Li-po なら 13.5 (Li-po は過放電するので注意！)
const component::Battery Battery(Vbatt, 13.0);

const component::LedLight NativeLed(PIN_NATIVE_LED);
const component::LedLight LineLed(PIN_LINE_LED);
const component::LedLight LedR(PIN_LED_R);
const component::LedLight LedY(PIN_LED_Y);
const component::LedLight LedG(PIN_LED_G);
const component::LedLight LedB(PIN_LED_B);
const component::LedLight BuiltinLed(LED_BUILTIN);

const component::XBee XBee(9600);
component::OpenMV OpenMV(19200);

void setup() {
    pinMode(StartSW, INPUT_PULLUP);

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

    digitalWrite(Kick1, LOW);
    digitalWrite(Kick_Dir, LOW);
    digitalWrite(SWR, HIGH);
    digitalWrite(SWG, HIGH);

    pinMode(INT_29, INPUT_PULLUP);  // interrupt port set

    Serial.begin(9600);
    Serial.println("Starting...");

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

    Serial.println("Initialize 3 ...");

    Serial2.begin(115200);  // WT901 IMU Sener

    // Caution D29 -> Interrupt5

    attachInterrupt(INT_29, intHandle, RISING);

    digitalWrite(SWR, HIGH);
    digitalWrite(SWG, HIGH);
    Serial.println("Initialize end");
}

void loop() {
    LedB.TernOff();
    OpenMV.wait_data();
    blob_count = OpenMV.blob_count();

    lineflag = false;

    // get gyrodata
    if (Serial2.available() > 0) {
        while (Serial2.available() != 0) {
            // Gyro の方位データを gyro に取り込む
            // Serial2の送信側がint8_tで送ってるので、intで受け取ると負の数が期待通り受け取れない。
            // そのため、int8_tにキャストする必要がある。
            gyro_o = (int8_t)Serial2.read();
        }
    }

    const int gyro = gyro_o;

    if (XBee.has_data()) {
        p_ball = XBee.read_data();
    }

    exist_ball = OpenMV.get_ball_count() != 0;
    ball_pos = OpenMV.get_ball_position();
    exist_blue_goal = OpenMV.get_blue_goal_count() != 0;
    blue_goal = OpenMV.get_blue_goal_position();
    exist_yellow_goal = OpenMV.get_yellow_goal_count() != 0;
    yellow_goal = OpenMV.get_yellow_goal_position();

    // 中心補正
    if (exist_ball) {
        ball_pos = Vector2(156, 67) - ball_pos;
    }

    if (digitalRead(GoalSW)) {  // 青色ゴールをする場合
        if (exist_yellow_goal) {
            yellow_goal.x = 154 - yellow_goal.x;
            yellow_goal.y = yellow_goal.y - 184;
        }
        if (exist_blue_goal) {
            blue_goal.x = 151 - blue_goal.x;
            blue_goal.y = blue_goal.y - 58;
        }
    } else {  // 黄色ゴールをする場合
        if (exist_yellow_goal) {
            yellow_goal.x = 151 - yellow_goal.x;
            yellow_goal.y = yellow_goal.y - 63;
        }
        if (exist_blue_goal) {
            blue_goal.x = 156 - blue_goal.x;
            blue_goal.y = blue_goal.y - 184;
        }
    }

    if (exist_ball) {
        int fixed_x = ball_pos.x > 4095 ? 4095 : ball_pos.x;
        int send_data = sqrt(fixed_x * fixed_x + ball_pos.y * ball_pos.y);
        XBee.send_data(send_data);
    }

    ball_dist = Vector2::norm(ball_pos);

    Serial.print("ball_pos.x:");
    Serial.print(ball_pos.x);
    Serial.print(" ,ball_pos.y:");
    Serial.print(ball_pos.y);
    Serial.print(" ,tof_front=");
    Serial.println(ball_front);
    Serial.print(" ,yellowgoal_x=");  // 青ゴールのx座標
    Serial.print(blue_goal.x);
    Serial.print(" ,yellowgoal_y=");  // 青ゴールのy座標
    Serial.print(blue_goal.y);
    Serial.print(" ,tan=");
    Serial.println(atan2(blue_goal.x, -blue_goal.y));

    ball_front = ToF_front.readRangeSingleMillimeters();

    if (digitalRead(StartSW) == LOW) {  // STartSW == Low でスタート
        digitalWrite(SWR, HIGH);
        digitalWrite(SWG, HIGH);

        if (Battery.is_emergency()) {
            Serial.println("");
            Serial.print("  Battery Low!: ");
            Serial.println(Battery.voltage());
            doOutofbound();
            LineLed.TernOff();
            motorFree();
            while (true) {
                digitalWrite(SWR, HIGH);
                digitalWrite(SWG, HIGH);
                delay(300);
                digitalWrite(SWR, LOW);
                digitalWrite(SWG, LOW);
                delay(300);
            }
        }

        LineLed.TernOn();  // ラインセンサのLEDを点灯
        if (lineflag) {
            lineflag = false;
        }
        // PID

        // 役割判定
        if (digitalRead(Aux1) == LOW) {
            attacker(gyro);
        } else if (digitalRead(Aux2) == LOW) {
            keeper(gyro);

        } else {
            if (p_ball <= ball_dist) {  // どちらがボールに近いか
                keeper(gyro);
            } else {
                attacker(gyro);
            }
        }
    } else {  // ロボット停止
        motorFree();
        dribbler1(0);
        dribbler2(0);
        LineLed.TernOff();  // ラインセンサのLEDを消灯
        digitalWrite(SWR, HIGH);
        digitalWrite(SWG, HIGH);
        wrap = 0;
    }
}

/**
 * rotation(-100:100)
 */
void keeper(const int rotation) {
    dribbler1(0);
    wrap = 0;

    Vector2 goal;
    bool exist_goal;
    if (digitalRead(GoalSW)) {  // 青色の場合
        exist_goal = exist_yellow_goal;
        goal = {-yellow_goal.x, yellow_goal.y};
    } else {  // 黄色の場合
        exist_goal = exist_blue_goal;
        goal = {-blue_goal.x, blue_goal.y};
    }

    BuiltinLed.TernOff();
    if (ball_dist - p_ball < 60 || exist_ball == false) {  // ボールとの距離の差が近い、ボールを任せてゴール前に帰る
        if (exist_goal == false) {
            motorfunction(PI, 100, -rotation);
        } else if (goal.y > 23) {  // ゴールから遠い
            float z = atan2(goal.x, goal.y - 23) + PI;
            motorfunction(z, 100, -rotation);
        } else if (goal.y < 23 && goal.y > 15 && abs(goal.x) > 33) {  // x座標が 0 から遠い
            float z = atan2(goal.x, goal.y - 23) + PI;
            motorfunction(z, 100, -rotation);
        } else if (goal.y < 15) {  // ゴールエリアの横にいるとき
            if (goal.x > 0) {
                motorfunction(-0.60, 60, 0);
            } else {
                motorfunction(0.60, 60, 0);
            }
        } else {  // ゴール前にいるとき
            motorfunction(0, 0, 0);
        }
    } else {  // ボールとの距離の差が遠い、自ら近づく
        float az = atan2(ball_pos.x, sqrt(ball_pos.y));
        motorfunction(az, sqrt(ball_pos.x * ball_pos.x + ball_pos.y * ball_pos.y / 4), -rotation);
    }
}

/**
 * rotation(-100:100)
 */
void attacker(const int rotation) {
    static float pre_dir = 0;   // 前回観測値
    static float data_sum = 0;  // 誤差(観測値)の累積値

    const float Pmax = Power;

    Vector2 goal;
    bool exist_goal;
    if (digitalRead(GoalSW)) {  // GoalSWは攻める方向をスイッチに入れる,
        // 相手ゴールの座標は機体中心
        exist_goal = exist_blue_goal;
        goal = {blue_goal.x, -blue_goal.y};
    } else {
        exist_goal = exist_yellow_goal;
        goal = {yellow_goal.x, -yellow_goal.y};
    }
    // PID制御をするので
    // 制御値 = 誤差(方位)値 + 誤差(方位)の時間積分値 + 誤差(方位)の時間微分値

    // Convert coordinates data
    float ball_dir = 0;
    if (blob_count != 0) {                  // 物体を検出したら
        const float fx = 150 - ball_pos.x;  // ロボットが原点に来るようjに座標を変換
        const float fy = 130 - ball_pos.y;

        if (fy > 0) {  // 正面から見たボールの方位(radian)を計算
            ball_dir = atan(fx / fy);
        } else if (fx > 0) {
            ball_dir = atan((-fy) / fx) + 1.571;  // 1.571 = Pi/2
        } else {
            ball_dir = atan((-fy) / fx) - 1.571;
        }
        ball_dir = ball_dir + 0.150;  // +0.150は製作誤差による方位のオフセット補正値(radian)
    }
    const float data_diff = ball_dir - pre_dir;                                       // 前回の方位との差分を計算
    data_sum += data_diff;                                                            // 方位誤差の累積を計算
    const float Pcontrol = Power * (Kp * ball_dir + Ki * data_sum + Kd * data_diff);  // PIDの制御値を計算
    pre_dir = ball_dir;                                                               // 今回の値を代入し次周期から見て前回観測値にする

    BuiltinLed.TernOff();

    static bool kick = false;
    if (-5 <= ball_pos.y && ball_pos.y <= 30) {  // ボールが前(0 <= y <= 0)にあるとき
        dribbler1(100);
        wrap = 0;
        if (abs(ball_pos.x) < 5) {       // 目の前
            if (ball_front <= 60) {      // y の距離近い
                if (ball_front <= 30) {  // 保持
                    data_sum = 0;
                    if (exist_goal == false) {  // ゴールなし
                        motorfunction(0, 80, -rotation);
                    } else if (goal.y <= 33 && abs(goal.x) < 17) {  // ゴールにけれる距離
                        kick = true;
                        digitalWrite(Kick_Dir, LOW);
                        dribbler1(0);
                        digitalWrite(Kicker, HIGH);
                        delay(200);
                        motorfunction(0, 0, 0);
                        delay(800);
                        digitalWrite(Kicker, LOW);
                    } else if (goal.y < 5) {                // ゴールに近づいた時
                        motorfunction(PI, 100, -rotation);  // 後ろに下がる
                    } else {                                // ゴール見えてて近くない
                        const float z = atan2(goal.x, goal.y);
                        motorfunction(z, powerLimit(Pmax, Pcontrol), -rotation);
                    }
                } else {  // 目の前のボールを保持しに行く
                    kick = false;
                    data_sum = 0;
                    motorfunction(0, 50, -rotation);
                }
            } else {
                const float z = atan2(ball_pos.x, ball_pos.y);
                motorfunction(z, powerLimit(Pmax, Pcontrol), -rotation);  // ココボール前 制御甘い？
            }
        } else {
            const float z = atan2(ball_pos.x, ball_pos.y);
            motorfunction(z, powerLimit(Pmax, Pcontrol), -rotation);
        }
    } else if (ball_pos.y <= 0) {  // 後ろにボールがあるとき
        dribbler1(0);
        if (abs(ball_pos.x) < 30) {
            if (ball_pos.y >= -129) {
                motorfunction(0, 50, -rotation);
                wrap = 0;
            } else if (ball_pos.y <= -150) {
                motorfunction(PI, abs(ball_pos.y) / 2.4, -rotation);
                wrap = 0;
            } else if (abs(ball_pos.x) < 5 + abs(ball_pos.y) / 5) {
                if (goal.x > 0 || wrap == 1) {
                    const float z = atan2(ball_pos.x + 800, ball_pos.y * 3);
                    motorfunction(z, Vector2::norm(ball_pos) + 10, -rotation);
                    wrap = 1;
                } else {
                    const float z = atan2(ball_pos.x - 800, ball_pos.y * 3);
                    motorfunction(z, Vector2::norm(ball_pos) + 10, -rotation);
                    wrap = 0;
                }
            } else {
                wrap = 0;
                const float z = atan2(ball_pos.x, ball_pos.y * 3);
                motorfunction(z, Vector2::norm(ball_pos) + 10, -rotation);
            }
        } else {
            wrap = 0;
            const float z = atan2(ball_pos.x, ball_pos.y * 4);
            motorfunction(z, Vector2::norm(ball_pos) + 10, -rotation);
        }
    } else {  // 30 > y になるとき
        dribbler1(0);
        dribbler2(0);
        wrap = 0;
        if (exist_ball == false) {  // ボールがないとき(y = 4096)
            motorfunction(0, 0, 0);
        } else {                              // ボールがあるとき
            motorfunction(0, 80, -rotation);  // これでたまに回り込みがおおげさになる？
        }
    }
    Serial.print(" dir ");
    Serial.print(ball_dir);
    Serial.print(" sum ");
    Serial.print(data_sum);
    Serial.print(" diff ");
    Serial.print(data_diff);
    Serial.print(" Pcontrol ");
    Serial.print(Pcontrol);
    Serial.print(" kick ");
    Serial.println(kick);
}

/**
 * powerの値がmax(ex.100)を超えないようにする
 *
 * Note: C++17 からは std::clamp() が使える
 */
int powerLimit(const int max, const int power) {
    if (power > max) {
        return max;
    } else if (power < -max) {
        return -max;
    }
    return power;
}

//*****************************************************************************
// interrupt handler
// 割り込みの処理プログラム
// Lineを踏んだらバックする

void intHandle() {  // Lineを踏んだらlineflagをセットして止まる。
    LedB.TernOn();

    if (digitalRead(StartSW) == HIGH) {  // スイッチがOFFなら何もしない。
        return;
    }

    constexpr int power = 30;

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
            LedR.TernOn();
        }
    }

    LedB.TernOff();
    LedR.TernOff();

    if (lineflag == false) {  // センサーの反応がない場合は何もしない
        return;
    }
    lineflag = true;  // set lineflag
    motorStop();      // ラインから外れたらモーターstop
    return;
}

void back_Line1(const int power) {  // Lineセンサ1が反応しなくなるまで後ろに進む
    float azimuth;
#if DEBUG_MODE
    LedR.TernOn();
#endif
    while ((digitalRead(LINE1D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE3D) == HIGH)) {
        if (digitalRead(LINE4D) == HIGH) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(1+4)をradianに変換
        } else if (digitalRead(LINE2D) == HIGH) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(1+2)をradianに変換
        } else {
            azimuth = PI * 4.0 / 4.0;  // 後ろ方向(3)をradianに変換
        }
        motorfunction(azimuth, power, 0);  // azimuthの方向に進ませる
    }
#if DEBUG_MODE
    LedR.TernOff();
#endif
    motorStop();
}

void back_Line2(const int power) {  // Lineセンサ2が反応しなくなるまで左に進む
    float azimuth;
#if DEBUG_MODE
    LedY.TernOn();
#endif
    while ((digitalRead(LINE2D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE4D) == HIGH)) {
        if (digitalRead(LINE1D) == HIGH) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(2+1)を radian に変換
        } else if (digitalRead(LINE3D) == HIGH) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(2+3)を radian に変換
        } else {
            azimuth = PI * 6.0 / 4.0;  // 後ろ方向(4)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedY.TernOff();
#endif
    motorStop();
}

void back_Line3(const int power) {  // Lineセンサ3 が反応しなくなるまで前に進む
    float azimuth;
#if DEBUG_MODE
    LedG.TernOn();
#endif
    while ((digitalRead(LINE3D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE1D) == HIGH)) {
        if (digitalRead(LINE4D) == HIGH) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(3+4)を radian に変換
        } else if (digitalRead(LINE2D) == HIGH) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(3+2)を radian に変換
        } else {
            azimuth = PI * 0.0 / 4.0;  // 後ろ方向(1)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedG.TernOff();
#endif
    motorStop();
}

void back_Line4(const int power) {  // Lineセンサ4 が反応しなくなるまで右に進む
    float azimuth;
#if DEBUG_MODE
    LedB.TernOn();
#endif
    while ((digitalRead(LINE4D) == HIGH) || (digitalRead(LINE5D) == HIGH) || (digitalRead(LINE2D) == HIGH)) {
        if (digitalRead(LINE3D) == HIGH) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(4+3)を radian に変換
        } else if (digitalRead(LINE1D) == HIGH) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(4+1)を radian に変換
        } else {
            azimuth = PI * 2.0 / 4.0;  // 後ろ方向(2)を radian に変換
        }
        motorfunction(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedB.TernOff();
#endif
    motorStop();
}

// 割り込みの処理プログラム終わり
//*****************************************************************************

// 強制的に Out of bounds させる。
void doOutofbound() {
    // Out of bounds するために割込みを禁止する
    detachInterrupt(5);

    LineLed.TernOff();

    while (true) {
        if (digitalRead(StartSW) == LOW) {
            motorfunction(PI / 2.0, 30, 0);
        } else {  // スタートスイッチが切られたら止まる
            motorfunction(PI / 2.0, 0, 0);
        }
        digitalWrite(SWG, LOW);
        digitalWrite(SWR, LOW);
        delay(25);
        digitalWrite(SWG, HIGH);
        digitalWrite(SWR, HIGH);
        delay(25);
    }
}
