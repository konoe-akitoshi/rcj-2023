// なぜか、inoファイルでset filetype=cppするとclangdが正常に機能しなくなるので、cppファイルで作業してinoにコピーする。
#ifdef PREVENT_SORCING_FROM_ARDUINO

// デバッグ: 1
// リリース: 0
#define DEBUG_MODE 1

#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.h"
#include "../local/vl6180x.hpp"
#include "../local/wire.hpp"
#else
#include <VL6180X.h>
#include <Wire.h>
#endif
#include "components/battery.hpp"
#include "components/led_light.hpp"
#include "components/open_mv.hpp"
#include "components/xbee.hpp"
#include "components/motor.hpp"
#include "components/dribbler.hpp"
#include "types/vector2.hpp"
#include "pin.hpp"

VL6180X ToF_front;  // create front ToF object

int blob_count;

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
void doOutofbound();

// 制御パラメータの設定
constexpr float Kp = 0.45;   // 比例要素の感度
constexpr float Ki = 0.1;    // 積分要素の感度
constexpr float Kd = 0.025;  // 微分要素の感度

// initial motor power
constexpr int Power = 70;

// Low limit voltage 1.1*12 = 13.2
// Mi-NH なら 13.0, Li-po なら 13.5 (Li-po は過放電するので注意！)
const component::Battery Battery(PIN_BATTERY_VOLYAGE, 13.0);

const component::LedLight NativeLed(PIN_NATIVE_LED);
const component::LedLight LineSensorLed(PIN_LINE_SENSOR_LED);
const component::LedLight LedR(PIN_LED_R);
const component::LedLight LedY(PIN_LED_Y);
const component::LedLight LedG(PIN_LED_G);
const component::LedLight LedB(PIN_LED_B);
const component::LedLight BuiltinLed(LED_BUILTIN);
const component::LedLight SwitchLedG(PIN_SWITCH_LED_G);
const component::LedLight SwitchLedR(PIN_SWITCH_LED_R);

// PWM = 37KHz
const component::Motor MotorCh1(PIN_MOTOR1_FORWARD_BRAKE, PIN_MOTOR1_REVERSE_BRAKE, PIN_MOTOR1_PWM, 37000);
const component::Motor MotorCh2(PIN_MOTOR2_FORWARD_BRAKE, PIN_MOTOR2_REVERSE_BRAKE, PIN_MOTOR2_PWM, 37000);
const component::Motor MotorCh3(PIN_MOTOR3_FORWARD_BRAKE, PIN_MOTOR3_REVERSE_BRAKE, PIN_MOTOR3_PWM, 37000);
const component::Motor MotorCh4(PIN_MOTOR4_FORWARD_BRAKE, PIN_MOTOR4_REVERSE_BRAKE, PIN_MOTOR4_PWM, 37000);
const component::MotorContoroler MotorContoroler(MotorCh1, MotorCh2, MotorCh3, MotorCh4);

const component::Dribbler Dribbler(PIN_DRIBBLER_PWM);

const component::XBee XBee(9600);
component::OpenMV OpenMV(19200);

enum class GoalType {
    Blue,
    Yellow
} target_goal_type;

void setup() {
    Serial.begin(9600);
    Serial.println("DONE open Serial(9600)");

    pinMode(PIN_START_SWITCH, INPUT_PULLUP);
    pinMode(PIN_GOAL_SWITCH, INPUT_PULLUP);
    pinMode(PIN_LINE_SENSOR_D1, INPUT_PULLUP);
    pinMode(PIN_LINE_SENSOR_D2, INPUT_PULLUP);
    pinMode(PIN_LINE_SENSOR_D3, INPUT_PULLUP);
    pinMode(PIN_LINE_SENSOR_D4, INPUT_PULLUP);
    pinMode(PIN_LINE_SENSOR_D5, INPUT_PULLUP);
    pinMode(PIN_KICKER, OUTPUT);
    pinMode(PIN_AUX1, INPUT);
    pinMode(PIN_AUX2, INPUT);
    pinMode(PIN_INTERRUPT_29, INPUT_PULLUP);
    Serial.println("DONE setup pin-mode");

    Wire.begin();
    Serial.println("DONE open Wire");

    // CE端子をLOWにするとデバイスがリセットされアドレスが初期値に戻るので注意
    delay(10);
    ToF_front.init();
    ToF_front.configureDefault();
    ToF_front.setAddress(0x52);  // 好きなアドレスに設定
    ToF_front.setTimeout(100);
    delay(10);
    Serial.println("DONE setup ToF_front");

    // Caution D29 -> Interrupt
    attachInterrupt(PIN_INTERRUPT_29, intHandle, RISING);
    Serial.print("DONE attach interrupt to pin(RISING): ");
    Serial.println(PIN_INTERRUPT_29);

    // Dribbler 動作テスト
    Dribbler.Start(100);
    delay(1000);
    Dribbler.Stop();
    Serial.println("DONE check dribbler");

    delay(1000);

    // Kicker 動作テスト
    digitalWrite(PIN_KICKER, LOW);
    delay(100);
    digitalWrite(PIN_KICKER, HIGH);
    delay(100);
    digitalWrite(PIN_KICKER, LOW);
    Serial.println("DONE check kicker");

    // WT901 IMU Sener
    Serial2.begin(115200);
    Serial.println("DONE open Serial2(115200)");

    SwitchLedR.TernOn();
    SwitchLedG.TernOn();
    Serial.println("DONE setup");
}

void loop() {
    static int gyro_o;

    LedB.TernOff();
    OpenMV.WaitData();
    blob_count = OpenMV.BlobCount();
    lineflag = false;

    // Gyro の方位データを gyro に取り込む
    if (Serial2.available() > 0) {
        while (Serial2.available() != 0) {
            // Serial2の送信側がint8_tで送ってるので、intで受け取ると負の数が期待通り受け取れない。
            // そのため、int8_tにキャストする必要がある。
            gyro_o = (int8_t)Serial2.read();
        }
    }
    const int gyro = gyro_o;

    if (XBee.HasData()) {
        p_ball = XBee.ReadData();
    }

    exist_ball = OpenMV.GetBallCount() != 0;
    ball_pos = OpenMV.GetBallPosition();
    exist_blue_goal = OpenMV.GetBlueGoalCount() != 0;
    blue_goal = OpenMV.GetBlueGoalPosition();
    exist_yellow_goal = OpenMV.GetYellowGoalCount() != 0;
    yellow_goal = OpenMV.GetYellowGoalPosition();

    target_goal_type = digitalRead(PIN_GOAL_SWITCH) == HIGH ? GoalType::Blue : GoalType::Yellow;

    // 中心補正
    if (exist_ball) {
        ball_pos = Vector2(156, 67) - ball_pos;
    }
    if(target_goal_type == GoalType::Blue) {
        if (exist_yellow_goal) {
            yellow_goal.x = 154 - yellow_goal.x;
            yellow_goal.y = yellow_goal.y - 184;
        }
        if (exist_blue_goal) {
            blue_goal.x = 151 - blue_goal.x;
            blue_goal.y = blue_goal.y - 58;
        }
    } else { // GoalType::Yellow
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
        XBee.SendData(send_data);
    }
    ball_dist = Vector2::Norm(ball_pos);

    ball_front = ToF_front.readRangeSingleMillimeters();

#if DEBUG_MODE
    Serial.print("ball_pos: ");
    Serial.print(ball_pos.x);
    Serial.print(", ");
    Serial.print(ball_pos.y);
    Serial.print(" / tof_front: ");
    Serial.print(ball_front);
    Serial.print(" / yellow_goal: ");
    Serial.print(yellow_goal.x);
    Serial.print(", ");
    Serial.print(yellow_goal.y);
    Serial.print(" / blue_goal: ");
    Serial.print(blue_goal.x);
    Serial.print(", ");
    Serial.print(blue_goal.y);
    Serial.print(" / tan: ");
    Serial.println(atan2(blue_goal.x, -blue_goal.y));
#endif

    SwitchLedR.TernOn();
    SwitchLedG.TernOn();

    // PIN_START_SWITCH == Low でスタート、それ以外はロボット停止
    if (digitalRead(PIN_START_SWITCH) != LOW) {
        MotorContoroler.FreeAll();
        Dribbler.Stop();
        LineSensorLed.TernOff();
        wrap = 0;
        return;
    }

    if (Battery.IsEmergency()) {
        Serial.print("!! Battery Low !!  Voltage: ");
        Serial.println(Battery.Voltage());
        doOutofbound();
        LineSensorLed.TernOff();
        MotorContoroler.FreeAll();
        while (true) {
            SwitchLedR.TernOff();
            SwitchLedG.TernOff();
            delay(300);
            SwitchLedR.TernOn();
            SwitchLedG.TernOn();
            delay(300);
        }
    }

    LineSensorLed.TernOn();

    // 役割判定
    if (digitalRead(PIN_AUX1) == LOW) {
        attacker(gyro);
    } else if (digitalRead(PIN_AUX2) == LOW) {
        keeper(gyro);
    } else {
        // どちらがボールに近いか
        if (p_ball <= ball_dist) {
            keeper(gyro);
        } else {
            attacker(gyro);
        }
    }
}

/**
 * rotation(-100:100)
 */
void keeper(const int rotation) {
    Dribbler.Stop();
    wrap = 0;

    Vector2 goal;
    bool exist_goal;
    if (target_goal_type == GoalType::Blue) {
        exist_goal = exist_yellow_goal;
        goal = {-yellow_goal.x, yellow_goal.y};
    } else {  // GoalType::Yellow
        exist_goal = exist_blue_goal;
        goal = {-blue_goal.x, blue_goal.y};
    }

    BuiltinLed.TernOff();
    if (ball_dist - p_ball < 60 || exist_ball == false) {  // ボールとの距離の差が近い、ボールを任せてゴール前に帰る
        if (exist_goal == false) {
            MotorContoroler.Drive(PI, 100, -rotation);
        } else if (goal.y > 23) {  // ゴールから遠い
            float z = atan2(goal.x, goal.y - 23) + PI;
            MotorContoroler.Drive(z, 100, -rotation);
        } else if (goal.y < 23 && goal.y > 15 && abs(goal.x) > 33) {  // x座標が 0 から遠い
            float z = atan2(goal.x, goal.y - 23) + PI;
            MotorContoroler.Drive(z, 100, -rotation);
        } else if (goal.y < 15) {  // ゴールエリアの横にいるとき
            if (goal.x > 0) {
                MotorContoroler.Drive(-0.60, 60, 0);
            } else {
                MotorContoroler.Drive(0.60, 60, 0);
            }
        } else {  // ゴール前にいるとき
            MotorContoroler.Drive(0, 0, 0);
        }
    } else {  // ボールとの距離の差が遠い、自ら近づく
        float az = atan2(ball_pos.x, sqrt(ball_pos.y));
        MotorContoroler.Drive(az, sqrt(ball_pos.x * ball_pos.x + ball_pos.y * ball_pos.y / 4), -rotation);
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
    if (target_goal_type == GoalType::Blue) {
        exist_goal = exist_blue_goal;
        goal = {blue_goal.x, -blue_goal.y};
    } else { // GoalType::Yellow
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
        Dribbler.Start(100);
        wrap = 0;
        if (abs(ball_pos.x) < 5) {       // 目の前
            if (ball_front <= 60) {      // y の距離近い
                if (ball_front <= 30) {  // 保持
                    data_sum = 0;
                    if (exist_goal == false) {  // ゴールなし
                        MotorContoroler.Drive(0, 80, -rotation);
                    } else if (goal.y <= 33 && abs(goal.x) < 17) {  // ゴールにけれる距離
                        kick = true;
                        Dribbler.Stop();
                        digitalWrite(PIN_KICKER, HIGH);
                        delay(200);
                        MotorContoroler.Drive(0, 0, 0);
                        delay(800);
                        digitalWrite(PIN_KICKER, LOW);
                    } else if (goal.y < 5) {                // ゴールに近づいた時
                        MotorContoroler.Drive(PI, 100, -rotation);  // 後ろに下がる
                    } else {                                // ゴール見えてて近くない
                        const float z = atan2(goal.x, goal.y);
                        MotorContoroler.Drive(z, powerLimit(Pmax, Pcontrol), -rotation);
                    }
                } else {  // 目の前のボールを保持しに行く
                    kick = false;
                    data_sum = 0;
                    MotorContoroler.Drive(0, 50, -rotation);
                }
            } else {
                const float z = atan2(ball_pos.x, ball_pos.y);
                MotorContoroler.Drive(z, powerLimit(Pmax, Pcontrol), -rotation);  // ココボール前 制御甘い？
            }
        } else {
            const float z = atan2(ball_pos.x, ball_pos.y);
            MotorContoroler.Drive(z, powerLimit(Pmax, Pcontrol), -rotation);
        }
    } else if (ball_pos.y <= 0) {  // 後ろにボールがあるとき
        Dribbler.Stop();
        if (abs(ball_pos.x) < 30) {
            if (ball_pos.y >= -129) {
                MotorContoroler.Drive(0, 50, -rotation);
                wrap = 0;
            } else if (ball_pos.y <= -150) {
                MotorContoroler.Drive(PI, abs(ball_pos.y) / 2.4, -rotation);
                wrap = 0;
            } else if (abs(ball_pos.x) < 5 + abs(ball_pos.y) / 5) {
                if (goal.x > 0 || wrap == 1) {
                    const float z = atan2(ball_pos.x + 800, ball_pos.y * 3);
                    MotorContoroler.Drive(z, Vector2::Norm(ball_pos) + 10, -rotation);
                    wrap = 1;
                } else {
                    const float z = atan2(ball_pos.x - 800, ball_pos.y * 3);
                    MotorContoroler.Drive(z, Vector2::Norm(ball_pos) + 10, -rotation);
                    wrap = 0;
                }
            } else {
                wrap = 0;
                const float z = atan2(ball_pos.x, ball_pos.y * 3);
                MotorContoroler.Drive(z, Vector2::Norm(ball_pos) + 10, -rotation);
            }
        } else {
            wrap = 0;
            const float z = atan2(ball_pos.x, ball_pos.y * 4);
            MotorContoroler.Drive(z, Vector2::Norm(ball_pos) + 10, -rotation);
        }
    } else {  // 30 > y になるとき
        Dribbler.Stop();
        wrap = 0;
        if (exist_ball == false) {  // ボールがないとき(y = 4096)
            MotorContoroler.Drive(0, 0, 0);
        } else {                              // ボールがあるとき
            MotorContoroler.Drive(0, 80, -rotation);  // これでたまに回り込みがおおげさになる？
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

    if (digitalRead(PIN_START_SWITCH) == HIGH) {  // スイッチがOFFなら何もしない。
        return;
    }

    constexpr int power = 30;

    while (digitalRead(PIN_INTERRUPT_29) == HIGH) {   // Lineセンサが反応している間は繰り返す
        if (digitalRead(PIN_LINE_SENSOR_D1) == HIGH) {  // lineを踏んだセンサーを調べる
            back_Line1(power);              // Lineセンサと反対方向へ移動する
            lineflag = true;                // set lineflag
        } else if (digitalRead(PIN_LINE_SENSOR_D2) == HIGH) {
            back_Line2(power);
            lineflag = true;  // set lineflag
        } else if (digitalRead(PIN_LINE_SENSOR_D3) == HIGH) {
            back_Line3(power);
            lineflag = true;  // set lineflag
        } else if (digitalRead(PIN_LINE_SENSOR_D4) == HIGH) {
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
    MotorContoroler.StopAll();      // ラインから外れたらモーターstop
    return;
}

void back_Line1(const int power) {  // Lineセンサ1が反応しなくなるまで後ろに進む
    float azimuth;
#if DEBUG_MODE
    LedR.TernOn();
#endif
    while ((digitalRead(PIN_LINE_SENSOR_D1) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D5) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D3) == HIGH)) {
        if (digitalRead(PIN_LINE_SENSOR_D4) == HIGH) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(1+4)をradianに変換
        } else if (digitalRead(PIN_LINE_SENSOR_D2) == HIGH) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(1+2)をradianに変換
        } else {
            azimuth = PI * 4.0 / 4.0;  // 後ろ方向(3)をradianに変換
        }
        MotorContoroler.Drive(azimuth, power, 0);  // azimuthの方向に進ませる
    }
#if DEBUG_MODE
    LedR.TernOff();
#endif
    MotorContoroler.StopAll();
}

void back_Line2(const int power) {  // Lineセンサ2が反応しなくなるまで左に進む
    float azimuth;
#if DEBUG_MODE
    LedY.TernOn();
#endif
    while ((digitalRead(PIN_LINE_SENSOR_D2) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D5) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D4) == HIGH)) {
        if (digitalRead(PIN_LINE_SENSOR_D1) == HIGH) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(2+1)を radian に変換
        } else if (digitalRead(PIN_LINE_SENSOR_D3) == HIGH) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(2+3)を radian に変換
        } else {
            azimuth = PI * 6.0 / 4.0;  // 後ろ方向(4)を radian に変換
        }
        MotorContoroler.Drive(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedY.TernOff();
#endif
    MotorContoroler.StopAll();
}

void back_Line3(const int power) {  // Lineセンサ3 が反応しなくなるまで前に進む
    float azimuth;
#if DEBUG_MODE
    LedG.TernOn();
#endif
    while ((digitalRead(PIN_LINE_SENSOR_D3) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D5) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D1) == HIGH)) {
        if (digitalRead(PIN_LINE_SENSOR_D4) == HIGH) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(3+4)を radian に変換
        } else if (digitalRead(PIN_LINE_SENSOR_D2) == HIGH) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(3+2)を radian に変換
        } else {
            azimuth = PI * 0.0 / 4.0;  // 後ろ方向(1)を radian に変換
        }
        MotorContoroler.Drive(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedG.TernOff();
#endif
    MotorContoroler.StopAll();
}

void back_Line4(const int power) {  // Lineセンサ4 が反応しなくなるまで右に進む
    float azimuth;
#if DEBUG_MODE
    LedB.TernOn();
#endif
    while ((digitalRead(PIN_LINE_SENSOR_D4) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D5) == HIGH) || (digitalRead(PIN_LINE_SENSOR_D2) == HIGH)) {
        if (digitalRead(PIN_LINE_SENSOR_D3) == HIGH) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(4+3)を radian に変換
        } else if (digitalRead(PIN_LINE_SENSOR_D1) == HIGH) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(4+1)を radian に変換
        } else {
            azimuth = PI * 2.0 / 4.0;  // 後ろ方向(2)を radian に変換
        }
        MotorContoroler.Drive(azimuth, power, 0);  // azimuth の方向に進ませる
    }
#if DEBUG_MODE
    LedB.TernOff();
#endif
    MotorContoroler.StopAll();
}

// 割り込みの処理プログラム終わり
//*****************************************************************************

// 強制的に Out of bounds させる。
void doOutofbound() {
    // Out of bounds するために割込みを禁止する
    detachInterrupt(5);

    LineSensorLed.TernOff();

    while (true) {
        if (digitalRead(PIN_START_SWITCH) == LOW) {
            MotorContoroler.Drive(PI / 2.0, 30, 0);
        } else {  // スタートスイッチが切られたら止まる
            MotorContoroler.Drive(PI / 2.0, 0, 0);
        }
        SwitchLedG.TernOff();
        SwitchLedR.TernOff();
        delay(25);
        SwitchLedG.TernOn();
        SwitchLedR.TernOn();
        delay(25);
    }
}

#endif
