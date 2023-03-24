#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "module/core/deps/vl6180x.hpp"
#else
#include <VL6180X.h>
#endif
#include "module/battery.hpp"
#include "module/camera.hpp"
#include "module/dribbler.hpp"
#include "module/gyro.hpp"
#include "module/kicker.hpp"
#include "module/led_light.hpp"
#include "module/line_sensors.hpp"
#include "module/motor_controller.hpp"
#include "module/switch.hpp"
#include "module/terminal.hpp"
#include "module/xbee.hpp"
#define SETUP_MODULE(x)                   \
    do {                                  \
        x.setup();                        \
        Serial.println("DONE setup " #x); \
    } while (0)
void attacker(void);
void keeper(void);
void interruptHandler(void);

VL6180X ToFSensor;

// 自分自身の状態
volatile float azimuth = 0;
int8_t rotation = 0;

// フィールドの状態
module::ObjectData ball;
module::ObjectData attack_goal;
module::ObjectData defence_goal;
int ball_front_dist = 255;
int pair_ball_dist = 255;

void setup() {
    Serial.begin(9600);
    delay(1000);

    SETUP_MODULE(Battery);
    SETUP_MODULE(MotorController);
    SETUP_MODULE(Camera);
    SETUP_MODULE(Gyro);
    SETUP_MODULE(XBee);
    SETUP_MODULE(Terminal);
    SETUP_MODULE(LineSensors);
    SETUP_MODULE(Dribbler);
    SETUP_MODULE(Kicker);
    SETUP_MODULE(StartSwitch);
    SETUP_MODULE(GoalSwitch);
    SETUP_MODULE(LineSensorLed);
    SETUP_MODULE(SwitchLedG);
    SETUP_MODULE(SwitchLedR);

    attachInterrupt(LineSensors.getInterruptPin(), interruptHandler, RISING);
    Serial.println("Attach interrupt");

    ToFSensor.setBus(&Wire);
    ToFSensor.init();
    ToFSensor.configureDefault();
    ToFSensor.setScaling(1);
    ToFSensor.setAddress(0x29);
    ToFSensor.setTimeout(100);
    Serial.println("DONE setup ToFSensor");

    // setup 完了通知
    SwitchLedG.turnOn();
    SwitchLedR.turnOn();

    // Dribbler 動作テスト
    Dribbler.start(100);
    delay(1000);
    Dribbler.stop();
    Serial.println("DONE check Dribbler");

    delay(1000);

    // Kicker 動作テスト
    Kicker.pushFront();
    delay(100);
    Kicker.pullBack();
    Serial.println("DONE check Kicker");

    // カメラが有効になるのを待つ
    SwitchLedG.turnOff();
    SwitchLedR.turnOn();
    while (true) {
        if (Camera.available()) {
            break;
        }
        Serial.println("Camera not available");
        for (int i = 0; i < 20; ++i) {
            Terminal.sendMachineStatus(Battery.voltage(), Gyro.getRotation());
            delay(50);
        }
    }
    SwitchLedG.turnOn();
    SwitchLedR.turnOff();
}

void loop() {
    if (Battery.isEmergency()) {
        Terminal.sendError();
        while (true) {
            SwitchLedG.turnOn();
            SwitchLedR.turnOff();
            Terminal.sendMachineStatus(Battery.voltage(), Gyro.getRotation());
            delay(200);
            SwitchLedG.turnOff();
            SwitchLedR.turnOn();
            Terminal.sendMachineStatus(Battery.voltage(), Gyro.getRotation());
            delay(200);
        }
    }

    // 状態取得
    const bool attack_blue_goal = GoalSwitch.isHigh();
    rotation = Gyro.getRotation();
    ball_front_dist = ToFSensor.readRangeSingleMillimeters();
    pair_ball_dist = (XBee.hasData() ? XBee.readData() : pair_ball_dist);
    ball = Camera.getBallData();
    const auto yellow_goal = Camera.getYellowGoalData();
    const auto blue_goal = Camera.getBlueGoalData();

    // 状態共有
    XBee.sendData(Vector2::norm(ball.position));

    if (StartSwitch.isHigh()) {
        // ストップ
        SwitchLedG.turnOn();
        Dribbler.stop();
        LineSensorLed.turnOff();
        MotorController.freeAll();
        Terminal.sendMachineStatus(Battery.voltage(), rotation);
        Terminal.sendFieldObjectData(ball, yellow_goal, blue_goal, ball_front_dist);
        delay(40);
        return;
    }

    // スタート
    SwitchLedG.turnOff();
    LineSensorLed.turnOn();

    attack_goal = (attack_blue_goal ? blue_goal : yellow_goal);
    defence_goal = (attack_blue_goal ? yellow_goal : blue_goal);

    if (ball.is_exist == false && ball_front_dist == 255) {
        MotorController.stopAll();
        return;
    }

    // ボールに近いが attacker になる
    attacker();

    // カメラのI2Cをやりすぎないように少し待つ
    delay(10);
}

void attacker() {
    static float pre_ball_dist = 0;

    const float ball_dist = Vector2::norm(ball.position);
    const auto ball_dist_diff = ball_dist - pre_ball_dist;
    pre_ball_dist = ball_dist;

    if (ball_front_dist < 255) {
        Dribbler.start(100);
    } else {
        Dribbler.stop();
    }

    if (ball_front_dist < 255) {  // FIXME: ToFSensor が直ったら適切な値にする
        // ボール保持している時
        if (attack_goal.is_exist == false) {
            // ゴールが見えない時は相手と接触している状態なので、全力で押す
            azimuth = PI / 2;
            MotorController.drive(azimuth, 100, -rotation);
            return;
        }
        // ゴールが見えている
        if (attack_goal.width >= 80 || attack_goal.position.y < 50) {
            // キックできる位置にいるのでキックする
            Dribbler.stop();
            Kicker.pushFront();
            delay(200);
            Kicker.pullBack();
            MotorController.drive(3 * PI / 2, 50, -rotation);  // リバンドに備える(下がるだけなのでazimuthの更新はしなくていい？)
        } else {
            // キックできる位置にいないので、ゴールへ向かう
            // ドリブラのホールドが弱いので、ゴール方向前方へ進む
            azimuth = Vector2::angle(attack_goal.position);
            MotorController.drive(azimuth, 50, -rotation);
        }
        return;
    }

    if (ball.position.y <= 3) {
        // ボールが後方にあるので、下がって回り込んでボールを取りにいく
        // ボールの偏角に45°を足した(引いた)方向に移動する
        const int cloc_wise = ball.position.x < 0 ? 1 : -1;
        Vector2 dir = Vector2::rotate(ball.position, cloc_wise * PI / 4);
        const int power = 30 + (1 * Vector2::norm(ball.position)) + (0.5 * ball_dist_diff);  // PD制御
        Serial.println(power);
        azimuth = Vector2::angle(dir);
        MotorController.drive(azimuth, power, -rotation);
        return;
    }

    // ボールから離れてるので、近づく
    const float power = 30 + (0.7 * Vector2::norm(ball.position)) + (0.7 * ball_dist_diff);  // PD制御
    azimuth = Vector2::angle(ball.position);
    MotorController.drive(azimuth, power, -rotation);
}

void keeper() {
    // ボールの方へ横移動
    volatile float azimuth = defence_goal.position.x > 0 ? 0 : PI;
    if (defence_goal.width >= 80) {
        // TODO: width のパラメータ調整
        // ゴールから近いときは前に進む
        // x のみずれても width が変わるので、不規則に動いてくれる？
        azimuth = (azimuth + (1 * PI / 2)) / 2;
    } else {
        azimuth = (azimuth + (3 * PI / 2)) / 2;
    }

    MotorController.drive(azimuth, 30, -rotation);
}

void interruptHandler() {
    if (StartSwitch.isHigh()) {
        return;
    }
    while (LineSensors.isLineDetected()) {
        const float az = (azimuth + LineSensors.calculateLineAzimut()) / 2;
        MotorController.drive(az + PI, 50, 0);
        delayMicroseconds(500);
    }
    MotorController.freeAll();
}
