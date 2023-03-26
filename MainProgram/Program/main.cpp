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
void forceOutOfBounds(void);

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
int ball_dist = 255;

// 状態変数
long xbee_lost_count = 0;

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

    ToFSensor.setBus(&Wire1);
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
        forceOutOfBounds();
    }

    // 状態取得
    const bool attack_blue_goal = GoalSwitch.isHigh();
    rotation = Gyro.getRotation();
    ball_front_dist = ToFSensor.readRangeSingleMillimeters();
    if (!XBee.hasData()) {
        xbee_lost_count += 1;
        if (xbee_lost_count > 50) {
            pair_ball_dist = 255;
        }
    } else {
        pair_ball_dist = XBee.readData();
        xbee_lost_count = 0;
    }
    ball = Camera.getBallData();
    const auto yellow_goal = Camera.getYellowGoalData();
    const auto blue_goal = Camera.getBlueGoalData();

    // ボールまでの距離を送信
    XBee.sendData(ball.is_exist ? Vector2::norm(ball.position) : 255);

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
    ball_dist = Vector2::norm(ball.position);

    // ボールに近いが attacker になる
    if (ball_dist <= pair_ball_dist) {
        attacker();
    } else {
        keeper();
    }

    // カメラのI2Cをやりすぎないように少し待つ
    delay(40);
}

void attacker() {
    static float pre_ball_dist = 0;
    const auto ball_dist_diff = ball_dist - pre_ball_dist;
    pre_ball_dist = ball_dist;

    if (ball.is_exist == false && ball_front_dist == 255) {
        MotorController.freeAll();
        return;
    }

    if (ball_front_dist < 255 || (abs(ball.position.x) <= 3 && 0 < ball.position.y && ball.position.y < 10)) {
        Dribbler.start(100);
    } else {
        Dribbler.stop();
    }

    if (ball_front_dist < 30) {
        // ボール保持している時
        if (attack_goal.is_exist == false) {
            // ゴールが見えない時は相手と接触している状態なので、全力で押す
            azimuth = PI / 2;
            MotorController.drive(azimuth, 100, -rotation);
            return;
        }

        if (attack_goal.position.y <= 10) {
            // ゴールに近すぎるので下がる
            int add_rotation = 0;
            if (Vector2::angle(attack_goal.position) > (2 * PI / 3)) {
                add_rotation -= 40;
            } else if (Vector2::angle(attack_goal.position) < (1 * PI / 3)) {
                add_rotation += 40;
            }
            MotorController.drive(3 * PI / 2, 30, -rotation + add_rotation);
            return;
        }

        // キックできる位置
        if (attack_goal.position.y < 70
            || (attack_goal.position.x <= 0 && rotation < -15)
            || (attack_goal.position.x >= 0 && rotation > 15)) {
            // 少しホールドする
            delay(100);
            // キックできる位置にいるのでキックする
            Dribbler.stop();
            MotorController.stopAll();
            delay(100);
            Kicker.pushFront();
            delay(100);
            Kicker.pullBack();

            // リバンドに備える(下がるだけなのでazimuthの更新はしなくていい？)
            MotorController.drive(3 * PI / 2, 50, 0);
            delay(200);
            MotorController.freeAll();
            delay(200);
            return;
        }

        // キックできる位置にいないので、ゴールへ向かう
        int add_rotation = 0;
        if (Vector2::angle(attack_goal.position) > (2 * PI / 3)) {
            add_rotation -= 40;
        } else if (Vector2::angle(attack_goal.position) < (1 * PI / 3)) {
            add_rotation += 40;
        }
        azimuth = Vector2::angle({attack_goal.position.x, attack_goal.position.y * 0.7f});  // TODO: パラメタ
        MotorController.drive(azimuth, 70, -rotation + add_rotation);
        return;
    }

    if (ball.position.y <= 3) {
        // ボールが後方にあるので、下がって回り込んでボールを取りにいく
        // ボールの偏角に45°を足した(引いた)方向に移動する
        const int clockwise = ball.position.x < 0 ? 1 : -1;
        Vector2 dir = Vector2::rotate({ball.position.x, ball.position.y - 10}, clockwise * PI / 4);
        const int power = 40 + (1 * Vector2::norm(ball.position)) + (0.5 * ball_dist_diff);  // PD制御
        Serial.println(power);
        azimuth = Vector2::angle(dir);
        MotorController.drive(azimuth, power, -rotation);
        return;
    }

    // ボールを吸う速度に落とす or 吸うのに失敗するのを一定回数以上行ったら蹴る

    // ボールから離れてるので、近づく
    const float power = 40 + (0.7 * Vector2::norm(ball.position)) + (0.7 * ball_dist_diff);  // PD制御
    azimuth = Vector2::angle({ball.position.x * 1.3f, ball.position.y});
    MotorController.drive(azimuth, power, -rotation);
}

void keeper() {
    if (ball.is_exist == false) {
        // ボールが見えないのでゴール前に戻る
        if (defence_goal.is_exist) {
            azimuth = Vector2::angle(defence_goal.position);
        } else {
            azimuth = -1 * PI / 2;
        }
        MotorController.drive(azimuth, 50, -rotation);
        return;
    }

    if (ball_dist > 60 + pair_ball_dist) {
        // ボールとの距離が十分遠い or ペアのロボットがボールに十分近い ので、自らボールに近づく
        // keeper となっている時点で、ball_dist > pair_ball_dist である
        const auto az = Vector2::angle({ball.position.x, ball.position.y * 0.7f});
        MotorController.drive(az, 50, -rotation);
        return;
    }

    // ボールの方へ横移動、 ゴールから近いときは前、遠い時は後ろ方向に進む
    // x のみずれても width が変わるので、不規則に動いてくれる？
    const bool forward = (defence_goal.width >= 50);
    if (ball.position.x > 0) {
        azimuth = (forward ? 1 : -1) * 1 * PI / 4;
    } else {
        azimuth = (forward ? 1 : -1) * 3 * PI / 4;
    }
    MotorController.drive(azimuth, 50, -rotation);
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

void forceOutOfBounds() {
    detachInterrupt(LineSensors.getInterruptPin());
    while (true) {
        MotorController.drive(0, 50, 0);
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
