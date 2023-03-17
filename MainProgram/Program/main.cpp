// デバッグ: 1
// リリース: 0
#define DEBUG_MODE 1

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "module/core/deps/vl6180x.hpp"
#else
#include <VL6180X.h>
#endif

#include <array>
#include "module/battery.hpp"
#include "module/camera.hpp"
#include "module/dribbler.hpp"
#include "module/gyro.hpp"
#include "module/kicker.hpp"
#include "module/led_light.hpp"
#include "module/line_sensors.hpp"
#include "module/motor_controller.hpp"
#include "module/switch.hpp"
#include "module/vector2.hpp"
#include "module/xbee.hpp"

#define SETUP_MODULE(x)                   \
    do {                                  \
        x.setup();                        \
        Serial.println("DONE setup " #x); \
    } while (0)

void keeper();
void attacker();
void interruptHandler();
void forceOutOfBounds();

VL6180X ToFSensor;

module::ObjectData ball;
module::ObjectData blue_goal;
module::ObjectData yellow_goal;
int ball_front;
int rotation;
// Vector2 velocity; // NEXT: impl
float ball_dist;
float pair_ball_dist = 255;  // NEXT: use Xbee

enum class GoalType
{
    Blue,
    Yellow
} target_goal_type;

void setup() {
    Serial.begin(9600);
    delay(1000);
    if (Serial) {
        Serial.println("DONE open Serial(9600)");
    } else {
        Serial.println("WARN Serial(9600) does not open");
    }

    SETUP_MODULE(Battery);
    SETUP_MODULE(Dribbler);
    SETUP_MODULE(Kicker);
    SETUP_MODULE(Camera);
    SETUP_MODULE(Gyro);
    SETUP_MODULE(MotorController);
    SETUP_MODULE(SwitchLedR);
    SETUP_MODULE(SwitchLedG);
    SETUP_MODULE(StartSwitch);
    SETUP_MODULE(LineSensorLed);
    SETUP_MODULE(LineSensors);

    Serial.println("DONE setup all modules");

    // CE端子をLOWにするとデバイスがリセットされアドレスが初期値に戻るので注意
    delay(10);
    ToFSensor.init();
    ToFSensor.configureDefault();
    ToFSensor.setAddress(0x52);  // 好きなアドレスに設定
    ToFSensor.setTimeout(100);
    delay(10);
    Serial.println("DONE setup ToFSensor");

    const int interrupt_pin = LineSensors.getInterruptPin();
    attachInterrupt(interrupt_pin, interruptHandler, RISING);
    Serial.print("DONE attach interrupt to pin(RISING): ");
    Serial.println(interrupt_pin);

    // Dribbler 動作テスト
    Dribbler.start(100);
    delay(1000);
    Dribbler.stop();
    Serial.println("DONE check dribbler");

    delay(1000);

    // Kicker 動作テスト
    Kicker.pushFront();
    delay(100);
    Kicker.pullBack();
    delay(1000);
    Serial.println("DONE check kicker");

    SwitchLedR.ternOn();
    SwitchLedG.ternOn();
    Serial.println("DONE setup");
}

void loop() {
    if (Battery.isEmergency()) {
        Serial.print("!! Battery Low !!  Voltage: ");
        Serial.println(Battery.voltage());
        forceOutOfBounds();
        LineSensorLed.ternOff();
        MotorController.freeAll();
        while (true) {
            SwitchLedR.ternOff();
            SwitchLedG.ternOff();
            delay(300);
            SwitchLedR.ternOn();
            SwitchLedG.ternOn();
            delay(300);
        }
    }

    rotation = Gyro.getRotation();

    ball = Camera.getBallData();
    blue_goal = Camera.getBlueGoalData();
    yellow_goal = Camera.getYellowGoalData();

    target_goal_type = GoalSwitch.isHigh() ? GoalType::Blue : GoalType::Yellow;

    // 機体のホールドエリアの座標(*)が(0, 0)になるように座標を変換する
    //    +x      (0,0)            +y
    //    |‾‾‾‾‾‾‾‾‾|          |‾‾‾‾‾‾‾‾‾|
    //    |  (?,?)  |          |  (0,0)  |
    //    |    *    |   =>  -x |    *    | +x
    //    |         |          |         |
    //    |_________|          |_________|
    //             +y              -y
    if (ball.is_exist) {
        ball.position = Vector2(150, 127) - ball.position;
    }
    if (yellow_goal.is_exist) {
        yellow_goal.position = Vector2(150, 127) - yellow_goal.position;
    }
    if (blue_goal.is_exist) {
        blue_goal.position = Vector2(150, 127) - blue_goal.position;
    }

    ball_dist = Vector2::norm(ball.position);
    ball_front = ToFSensor.readRangeSingleMillimeters();

    // Start Switch が Low でスタート、それ以外はロボット停止
    if (StartSwitch.isHigh()) {
        MotorController.freeAll();
        Dribbler.stop();
        LineSensorLed.ternOff();
        SwitchLedG.ternOff();
        return;
    }
    SwitchLedG.ternOn();

    LineSensorLed.ternOn();

    if (ball.is_exist == false && ball_front == 255) {
        MotorController.stopAll();
        return;
    }

    // ボールに近いが attacker になる
    if (ball_dist < pair_ball_dist) {
        attacker();
    } else {
        keeper();
    }
}

void keeper() {
    Dribbler.stop();

    const auto goal = target_goal_type == GoalType::Blue ? yellow_goal : blue_goal;
    const auto exist_goal = target_goal_type == GoalType::Blue ? yellow_goal.is_exist : blue_goal.is_exist;

    if (exist_goal == false || ball.is_exist == false) {
        // ゴールから離れているのでゴールまで後進する
        MotorController.drive(3 * PI / 2, 50, -rotation);
        return;
    }

    // NEXT: ボールが後ろにある時の動作
    //       横移動のみして、相手がボールに向かってくるのを邪魔する + ボールに触れないようにする
    if (ball.position.y < 0) {
        // 後ろにボールがある時は、諦める
        MotorController.stopAll();
        return;
    }

    if (ball_dist > 60 + pair_ball_dist) {
        // ボールとの距離が十分遠い or ペアのロボットがボールに十分近い ので、自らボールに近づく
        // keeper となっている時点で、ball_dist > pair_ball_dist である
        const auto az = Vector2::angle(ball.position);
        const auto pw = sqrt((ball.position.x * ball.position.x) + (ball.position.y * ball.position.y / 4));
        MotorController.drive(az, pw, -rotation);
        return;
    }

    if (goal.position.y > 15) {
        // ゴール横にいるので、ゴール前の角に移動するように動く
        // 機体がゴール前の角にいる時の goal の座標を target とすると、進む方向のベクトル dir は dir = goal - target で求まる
        const auto target = goal.position.x < 0 ? Vector2(-10, -5) : Vector2(10, -5);
        const auto dir = goal.position - target;
        const auto pw = 100;  // NEXT: velocity が導入されたら PD 制御する
        MotorController.drive(Vector2::angle(dir), pw, -rotation);
        return;
    }

    // あとはゴール前でボールがゴールに入らないように守る
    // だたし、1次元的な動きのみはルール違反になるので、ボールとの距離をみて少し前後にも動くようにする
    const auto dir = Vector2(ball.position.x, 5 + max(0, 0.5 * (ball_dist - 100)));
    MotorController.drive(Vector2::angle(dir), 50, -rotation);
}

void attacker() {
    static float pre_ball_dist = 0;

    const auto goal = target_goal_type == GoalType::Blue ? blue_goal : yellow_goal;
    const auto goal_width = target_goal_type == GoalType::Blue ? blue_goal.width : yellow_goal.width;
    const auto exist_goal = target_goal_type == GoalType::Blue ? blue_goal.is_exist : yellow_goal.is_exist;

    // NEXT: velocity導入されたら、それを使うようにする
    const auto ball_dist_diff = ball_dist - pre_ball_dist;

    if (ball.is_exist == false) {
        // ボールが見えていないがこの時 ball_front < 255 なので、前方に直進する
        MotorController.drive(PI / 2, 30, -rotation);
        return;
    }

    if (ball.position.y > 10 || abs(ball.position.x) > 30) {
        // ボールから離れてるので、近づく
        Dribbler.stop();
        const float pw = (0.1 * ball_dist) + (0.01 * ball_dist_diff);  // PD制御
        MotorController.drive(Vector2::angle(ball.position), pw, -rotation);
        return;
    }

    if (ball.position.y <= 0) {
        // ボールが後方近くにあるので、下がって回り込んでボールを取りにいく
        // 機体はボールの方向に垂直な方向に進んで回り込む
        // この時ラインを踏まないようにコートの中心側(ラインから遠い方)を通るように回り込む
        Dribbler.stop();
        Vector2 dir;
        if (exist_goal) {
            dir.x = goal.position.x > 0 ? 1 : -1;
        } else {
            const auto goal2 = target_goal_type == GoalType::Yellow ? blue_goal.position : yellow_goal.position;
            dir.x = goal2.x > 0 ? 1 : -1;
        }
        dir.y = dir.x * -1 * ball.position.x / ball.position.y;
        const auto pw = 40 + (0.1 * ball.position.y);  // 若干楕円軌道を描くように調整
        MotorController.drive(Vector2::angle(dir), pw, -rotation);
        return;
    }

    // ボールが前方近くにあるのでボールを保持しに行く or 保持し続ける
    Dribbler.start(100);

    if (ball_front > 30) {
        // ボールを保持してないので、目の前のボールを保持しに行く
        const auto az = Vector2::angle(ball.position);
        const auto pw = Vector2::norm(ball.position);
        MotorController.drive(az, pw, -rotation);
        return;
    }

    // NEXT: 相手に接触 and ゴールに近い (= 両方のゴールが見えない時?)
    //         -> ランダムに左右に揺れる + 少し後ろに

    if (exist_goal == false) {
        // ゴールから離れているので、直進
        MotorController.drive(PI / 2, 40, -rotation);
        return;
    }

    const float goal_center1 = float(goal.position.x - goal_width) / 2;
    const float goal_center2 = float(goal.position.x + goal_width) / 2;
    if (goal.position.y < 120 && goal_center1 * goal_center2 < 0 && min(abs(goal_center1), abs(goal_center2)) > 25) {
        // ゴールにボールを蹴れる距離にいるので、ボールを蹴る
        Dribbler.stop();
        Kicker.pushFront();
        delay(500);
        Kicker.pullBack();
        MotorController.drive(3 * PI / 2, 50, -rotation);  // 蹴ったあとリバウンドに備えて少し下がる
        return;
    }

    if (goal.position.y < 80) {
        // ゴール横にいるので、ゴール前の角に移動するように動く
        // 機体がゴール前の角にいる時の goal の座標を target とすると、進む方向のベクトル dir は dir = goal - target で求まる
        const auto target = goal.position.x < 0 ? Vector2(-50, 95) : Vector2(50, 95);
        const auto dir = goal.position - target;
        MotorController.drive(Vector2::angle(dir), 50, -rotation);
        return;
    }

    // ゴールに向かう
    const auto az = Vector2::angle(goal.position);
    const auto pw = Vector2::norm(goal.position);
    MotorController.drive(az, pw, -rotation);
}

void interruptHandler() {
    if (StartSwitch.isHigh()) {
        // スイッチがOFFなら何もしない。
        return;
    }

    while (LineSensors.isLineDetected()) {
        // ラインと反対方向へ移動する
        const auto data = LineSensors.calculateLineAzimut();
        float az = data.first;
        if (az > 10) {
            continue;
        }
        const auto index = data.second;
        while (LineSensors[index.front].isHigh() || LineSensors[index.center].isHigh() || LineSensors[index.back].isHigh()) {
            if (LineSensors[index.left].isHigh()) {
                az += -3 * PI / 4;
            } else if (LineSensors[index.right].isHigh()) {
                az += 3 * PI / 4;
            } else {
                az += PI;
            }
            MotorController.drive(az, 30, 0);
            delay(20);
        }
    }

    return;
}

void forceOutOfBounds() {
    // Out of bounds するために割込みを禁止する
    detachInterrupt(5);

    LineSensorLed.ternOff();

    while (true) {
        // スタートスイッチが切られたら止まる
        if (StartSwitch.isLow()) {
            MotorController.drive(PI, 30, 0);
        } else {
            MotorController.stopAll();
        }
        SwitchLedG.ternOff();
        SwitchLedR.ternOff();
        delay(25);
        SwitchLedG.ternOn();
        SwitchLedR.ternOn();
        delay(25);
    }
}
