// なぜか、inoファイルでset filetype=cppするとclangdが正常に機能しなくなるので、cppファイルで作業してinoにコピーする。
#ifdef PREVENT_SORCING_FROM_ARDUINO

// デバッグ: 1
// リリース: 0
#define DEBUG_MODE 1

#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#include "../local/vl6180x.hpp"
#else
#include <VL6180X.h>
#endif
#include "components/battery.hpp"
#include "components/digital_reader.hpp"
#include "components/dribbler.hpp"
#include "components/gyro.hpp"
#include "components/kicker.hpp"
#include "components/led_light.hpp"
#include "components/motor_controller.hpp"
#include "components/open_mv.hpp"
#include "components/setup_handler.hpp"
#include "components/xbee.hpp"
#include "pin.hpp"
#include "types/vector2.hpp"

void keeper();
void attacker();
void interruptHandler();
void back_Line1(const int power);
void back_Line2(const int power);
void back_Line3(const int power);
void back_Line4(const int power);
void forceOutOfBounds();

constexpr int INTERRUPT_PIN = raspberry_pi_pico::PIN34_GP28;

component::SetupHandler SetupHandler;

// Low limit voltage 1.1*12 = 13.2
// Mi-NH なら 13.0, Li-po なら 13.5 (Li-po は過放電するので注意！)
const component::Battery Battery(raspberry_pi_pico::PIN32_GP27, 13.0);

const component::LedLight LedR(SetupHandler, pcf8574::EX07);
const component::LedLight LedY(SetupHandler, pcf8574::EX05);
const component::LedLight LedG(SetupHandler, pcf8574::EX06);
const component::LedLight LedB(SetupHandler, pcf8574::EX04);
const component::LedLight SwitchLedR(SetupHandler, pcf8574::EX02);
const component::LedLight SwitchLedG(SetupHandler, pcf8574::EX03);
const component::LedLight LineSensorLed(SetupHandler, raspberry_pi_pico::PIN27_GP21);

const component::DigitalReader StartSwitch(SetupHandler, raspberry_pi_pico::PIN04_GP02, INPUT_PULLUP);
const component::DigitalReader GoalSwitch(SetupHandler, raspberry_pi_pico::PIN05_GP03, INPUT_PULLUP);

const component::DigitalReader LineSensorD1(SetupHandler, raspberry_pi_pico::PIN21_GP16, INPUT_PULLUP);
const component::DigitalReader LineSensorD2(SetupHandler, raspberry_pi_pico::PIN22_GP17, INPUT_PULLUP);
const component::DigitalReader LineSensorD3(SetupHandler, raspberry_pi_pico::PIN24_GP18, INPUT_PULLUP);
const component::DigitalReader LineSensorD4(SetupHandler, raspberry_pi_pico::PIN25_GP19, INPUT_PULLUP);
const component::DigitalReader LineSensorD5(SetupHandler, raspberry_pi_pico::PIN26_GP20, INPUT_PULLUP);

const component::DigitalReader AUX1(SetupHandler, pcf8574::EX00, INPUT);
const component::DigitalReader AUX2(SetupHandler, pcf8574::EX01, INPUT);

const component::MotorController MotorController(SetupHandler, raspberry_pi_pico::PIN14_GP10, raspberry_pi_pico::PIN15_GP11, 0x0A);
const component::Kicker Kicker(SetupHandler, raspberry_pi_pico::PIN16_GP12);
const component::Dribbler Dribbler(SetupHandler, raspberry_pi_pico::PIN17_GP13);
const component::Gyro Gyro(SetupHandler, 115200);
component::OpenMV OpenMV(SetupHandler, 19200);

VL6180X ToFSensor;

Vector2 ball_pos;
Vector2 blue_goal;
Vector2 yellow_goal;
bool exist_ball;
bool exist_yellow_goal;
bool exist_blue_goal;
int ball_front;
int rotation;
// Vector2 velocity; // NEXT: impl
float ball_dist;
float pair_ball_dist = 255; // NEXT: use Xbee

enum class GoalType
{
    Blue,
    Yellow
} target_goal_type;

void setup() {
    Serial.begin(9600);
    Serial.println("DONE open Serial(9600)");

    SetupHandler.Setup();
    Serial.println("DONE setup components");

    // CE端子をLOWにするとデバイスがリセットされアドレスが初期値に戻るので注意
    delay(10);
    ToFSensor.init();
    ToFSensor.configureDefault();
    ToFSensor.setAddress(0x52);  // 好きなアドレスに設定
    ToFSensor.setTimeout(100);
    delay(10);
    Serial.println("DONE setup ToF_front");

    // Caution D29 -> Interrupt
    pinMode(INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(INTERRUPT_PIN, interruptHandler, RISING);
    Serial.print("DONE attach interrupt to pin(RISING): ");
    Serial.println(INTERRUPT_PIN);

    // Dribbler 動作テスト
    Dribbler.Start(100);
    delay(1000);
    Dribbler.Stop();
    Serial.println("DONE check dribbler");

    delay(1000);

    // Kicker 動作テスト
    Kicker.PushFront();
    delay(100);
    Kicker.PullBack();
    Serial.println("DONE check kicker");

    SwitchLedR.TernOn();
    SwitchLedG.TernOn();
    Serial.println("DONE setup");
}

void loop() {
    OpenMV.WaitData();
    rotation = Gyro.GetRotation();

    exist_ball = OpenMV.GetBallCount() != 0;
    ball_pos = OpenMV.GetBallPosition();
    exist_blue_goal = OpenMV.GetBlueGoalCount() != 0;
    blue_goal = OpenMV.GetBlueGoalPosition();
    exist_yellow_goal = OpenMV.GetYellowGoalCount() != 0;
    yellow_goal = OpenMV.GetYellowGoalPosition();

    target_goal_type = GoalSwitch.IsHigh() ? GoalType::Blue : GoalType::Yellow;

    // 機体のホールドエリアの座標(*)が(0, 0)になるように座標を変換する
    //    +x      (0,0)            +y
    //    |‾‾‾‾‾‾‾‾‾|          |‾‾‾‾‾‾‾‾‾|
    //    |  (?,?)  |          |  (0,0)  |
    //    |    *    |   =>  -x |    *    | +x
    //    |         |          |         |
    //    |_________|          |_________|
    //             +y              -y
    //  (なんか変換方法おかしい気がするので、実機と合わせて確認しながら調整する)
    //  (多分、物体によって画像認識の誤差があるので個別調整が必要。だけどGoalTypeによって値が違うのが謎)
    //  (送られてくる座標の取り方が間違ってるかも、いずれにしても変換後は変わらない)
    //  (それと、おそらくrotationも考慮して変換後の向きを調整しないといけない)
    if (exist_ball) {
        ball_pos = Vector2(156, 67) - ball_pos;
    }
    if (target_goal_type == GoalType::Blue) {
        if (exist_yellow_goal) {
            yellow_goal.x = 154 - yellow_goal.x;
            yellow_goal.y = yellow_goal.y - 184;
        }
        if (exist_blue_goal) {
            blue_goal.x = 151 - blue_goal.x;
            blue_goal.y = blue_goal.y - 58;
        }
    } else {  // GoalType::Yellow
        if (exist_yellow_goal) {
            yellow_goal.x = 151 - yellow_goal.x;
            yellow_goal.y = yellow_goal.y - 63;
        }
        if (exist_blue_goal) {
            blue_goal.x = 156 - blue_goal.x;
            blue_goal.y = blue_goal.y - 184;
        }
    }

    ball_dist = Vector2::Norm(ball_pos);
    ball_front = ToFSensor.readRangeSingleMillimeters();

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

    // Start Switch が Low でスタート、それ以外はロボット停止
    if (StartSwitch.IsHigh()) {
        MotorController.FreeAll();
        Dribbler.Stop();
        LineSensorLed.TernOff();
        return;
    }

    if (Battery.IsEmergency()) {
        Serial.print("!! Battery Low !!  Voltage: ");
        Serial.println(Battery.Voltage());
        forceOutOfBounds();
        LineSensorLed.TernOff();
        MotorController.FreeAll();
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

    if (exist_ball == false) {
        MotorController.StopAll();
        return;
    }

    // 役割判定
    if (AUX1.IsLow()) {
        attacker();
    } else if (AUX2.IsLow()) {
        keeper();
    } else {
        // ボールに近いが attacker になる
        if (ball_dist < pair_ball_dist) {
            attacker();
        } else {
            keeper();
        }
    }
}

void keeper() {
    Dribbler.Stop();

    const auto goal = target_goal_type == GoalType::Blue ? yellow_goal : blue_goal;
    const auto exist_goal = target_goal_type == GoalType::Blue ? exist_yellow_goal : exist_blue_goal;

    if (exist_goal == false) {
        // ゴールから離れているのでゴールまで後進する
        MotorController.Drive(3 * PI / 2, 100, -rotation);
        return;
    }

    // NEXT: ボールが後ろにある時の動作
    //       横移動のみして、相手がボールに向かってくるのを邪魔する + ボールに触れないようにする
    if (ball_pos.y < 0) {
        // 後ろにボールがある時は、諦める
        MotorController.StopAll();
        return;
    }

    if (ball_dist > 60 + pair_ball_dist) {
        // ボールとの距離が十分遠い or ペアのロボットがボールに十分近い ので、自らボールに近づく
        // keeper となっている時点で、ball_dist > pair_ball_dist である
        const auto az = Vector2::Angle(ball_pos);
        const auto pw = sqrt((ball_pos.x * ball_pos.x) + (ball_pos.y * ball_pos.y / 4));
        MotorController.Drive(az, pw, -rotation);
        return;
    }

    if (goal.y > 15) {
        // ゴール横にいるので、ゴール前の角に移動するように動く
        // 機体がゴール前の角にいる時の goal の座標を target とすると、進む方向のベクトル dir は dir = goal - target で求まる
        const auto target = goal.x < 0 ? Vector2(-10, -5) : Vector2(10, -5);
        const auto dir = goal - target;
        const auto pw = 100;  // NEXT: velocity が導入されたら PD 制御する
        MotorController.Drive(Vector2::Angle(dir), pw, -rotation);
        return;
    }

    // あとはゴール前でボールがゴールに入らないように守る
    // だたし、1次元的な動きのみはルール違反になるので、ボールとの距離をみて少し前後にも動くようにする
    const auto dir = Vector2(ball_pos.x, goal.y + 5 + max(0, 0.5 * (ball_dist - 20)));
    MotorController.Drive(Vector2::Angle(dir), 100, -rotation);
}

void attacker() {
    static float pre_ball_dist = 0;

    const auto goal = target_goal_type == GoalType::Blue ? blue_goal : yellow_goal;
    const auto exist_goal = target_goal_type == GoalType::Blue ? exist_blue_goal : exist_yellow_goal;

    // NEXT: velocity導入されたら、それを使うようにする
    const auto ball_dist_diff = ball_dist - pre_ball_dist;
#if DEBUG_MODE
    Serial.print("[attacker] pre_ball_dist: ");
    Serial.println(pre_ball_dist);
#endif

    if (ball_pos.y > 10 || abs(ball_pos.x) > 30) {
        // ボールから離れてるので、近づく
        Dribbler.Stop();
        const float pw = (0.1 * ball_dist) + (0.01 * ball_dist_diff);  // PD制御
        MotorController.Drive(Vector2::Angle(ball_pos), pw, -rotation);
        return;
    }

    if (ball_pos.y <= 0) {
        // ボールが後方近くにあるので、下がって回り込んでボールを取りにいく
        // 機体はボールの方向に垂直な方向に進んで回り込む
        // この時ラインを踏まないようにコートの中心側(ラインから遠い方)を通るように回り込む
        Dribbler.Stop();
        Vector2 dir;
        if (exist_goal) {
            dir.x = goal.x > 0 ? 1 : -1;
        } else {
            const auto goal2 = target_goal_type == GoalType::Yellow ? blue_goal : yellow_goal;
            dir.x = goal2.x > 0 ? 1 : -1;
        }
        dir.y = dir.x * -1 * ball_pos.x / ball_pos.y;
        const auto pw = 80 + (0.1 * ball_pos.y);  // 若干楕円軌道を描くように調整
        MotorController.Drive(Vector2::Angle(dir), pw, -rotation);
        return;
    }

    // ボールが前方近くにあるのでボールを保持しに行く or 保持し続ける
    Dribbler.Start(100);

    if (ball_front > 30) {
        // ボールを保持してないので、目の前のボールを保持しに行く
        const auto az = Vector2::Angle(ball_pos);
        const auto pw = Vector2::Norm(ball_pos);
        MotorController.Drive(az, pw, -rotation);
        return;
    }

    // NEXT: 相手に接触 and ゴールに近い (= 両方のゴールが見えない時?)
    //         -> ランダムに左右に揺れる + 少し後ろに

    if (exist_goal == false) {
        // ゴールから離れているので、直進
        MotorController.Drive(PI / 2, 80, -rotation);
        return;
    }

    if (goal.y <= 33 && abs(goal.x) < 17) {
        // ゴールにボールを蹴れる距離にいるので、ボールを蹴る
        Dribbler.Stop();
        Kicker.PushFront();
        delay(500);
        Kicker.PullBack();
        MotorController.Drive(3 * PI / 2, 100, -rotation);  // 蹴ったあとリバウンドに備えて少し下がる
        return;
    }

    if (goal.y < 5) {
        // ゴール横にいるので、ゴール前の角に移動するように動く
        // 機体がゴール前の角にいる時の goal の座標を target とすると、進む方向のベクトル dir は dir = goal - target で求まる
        const auto target = goal.x < 0 ? Vector2(-10, 5) : Vector2(10, 5);
        const auto dir = goal - target;
        MotorController.Drive(Vector2::Angle(dir), 100, -rotation);
        return;
    }

    // ゴールに向かう
    const auto az = Vector2::Angle(goal);
    const auto pw = Vector2::Norm(goal);
    MotorController.Drive(az, pw, -rotation);
}

// TODO: back_Line. の動く方向が壊れてるのでなおす
void interruptHandler() {
    if (StartSwitch.IsHigh()) {  // スイッチがOFFなら何もしない。
        return;
    }

    // Lineセンサが反応している間は繰り返す
    while (digitalRead(INTERRUPT_PIN) == HIGH) {
        // lineを踏んだセンサーを調べ、Lineセンサと反対方向へ移動する
        if (LineSensorD1.IsHigh()) {
            back_Line1(30);
        } else if (LineSensorD2.IsHigh()) {
            back_Line2(30);
        } else if (LineSensorD3.IsHigh()) {
            back_Line3(30);
        } else if (LineSensorD4.IsHigh()) {
            back_Line4(30);
        } else {
            LedR.TernOn();
        }
    }

    MotorController.StopAll();  // ラインから外れたらモーターstop
    return;
}

void back_Line1(const int power) {  // Lineセンサ1が反応しなくなるまで後ろに進む
    float azimuth;
    while (LineSensorD1.IsHigh() || LineSensorD5.IsHigh() || LineSensorD3.IsHigh()) {
        if (LineSensorD4.IsHigh()) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(1+4)をradianに変換
        } else if (LineSensorD2.IsHigh()) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(1+2)をradianに変換
        } else {
            azimuth = PI * 4.0 / 4.0;  // 後ろ方向(3)をradianに変換
        }
        MotorController.Drive(azimuth, power, 0);
    }
    MotorController.StopAll();
}

void back_Line2(const int power) {  // Lineセンサ2が反応しなくなるまで左に進む
    float azimuth;
    while (LineSensorD2.IsHigh() || LineSensorD5.IsHigh() || LineSensorD4.IsHigh()) {
        if (LineSensorD1.IsHigh()) {
            azimuth = PI * 5.0 / 4.0;  // 後ろ方向(2+1)を radian に変換
        } else if (LineSensorD3.IsHigh()) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(2+3)を radian に変換
        } else {
            azimuth = PI * 6.0 / 4.0;  // 後ろ方向(4)を radian に変換
        }
        MotorController.Drive(azimuth, power, 0);
    }
    MotorController.StopAll();
}

void back_Line3(const int power) {  // Lineセンサ3 が反応しなくなるまで前に進む
    float azimuth;
    while (LineSensorD3.IsHigh() || LineSensorD5.IsHigh() || LineSensorD1.IsHigh()) {
        if (LineSensorD4.IsHigh()) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(3+4)を radian に変換
        } else if (LineSensorD2.IsHigh()) {
            azimuth = PI * 7.0 / 4.0;  // 後ろ方向(3+2)を radian に変換
        } else {
            azimuth = PI * 0.0 / 4.0;  // 後ろ方向(1)を radian に変換
        }
        MotorController.Drive(azimuth, power, 0);
    }
    MotorController.StopAll();
}

void back_Line4(const int power) {  // Lineセンサ4 が反応しなくなるまで右に進む
    float azimuth;
    while (LineSensorD4.IsHigh() || LineSensorD5.IsHigh() || LineSensorD2.IsHigh()) {
        if (LineSensorD3.IsHigh()) {
            azimuth = PI * 1.0 / 4.0;  // 後ろ方向(4+3)を radian に変換
        } else if (LineSensorD1.IsHigh()) {
            azimuth = PI * 3.0 / 4.0;  // 後ろ方向(4+1)を radian に変換
        } else {
            azimuth = PI * 2.0 / 4.0;  // 後ろ方向(2)を radian に変換
        }
        MotorController.Drive(azimuth, power, 0);
    }
    MotorController.StopAll();
}

void forceOutOfBounds() {
    // Out of bounds するために割込みを禁止する
    detachInterrupt(5);

    LineSensorLed.TernOff();

    while (true) {
        // スタートスイッチが切られたら止まる
        if (StartSwitch.IsLow()) {
            MotorController.Drive(PI, 30, 0);
        } else {
            MotorController.StopAll();
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
