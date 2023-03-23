#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "module/core/deps/vl6180x.hpp"
#else
#include <VL6180X.h>
#endif
#include "module/battery.hpp"
#include "module/camera.hpp"
#include "module/gyro.hpp"
#include "module/dribbler.hpp"
#include "module/kicker.hpp"
#include "module/led_light.hpp"
#include "module/line_sensors.hpp"
#include "module/motor_controller.hpp"
#include "module/switch.hpp"
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
int ball_front_dist = 255;

// フィールドの状態
module::ObjectData ball;
module::ObjectData blue_goal;
module::ObjectData yellow_goal;

enum class GoalType
{
    Blue,
    Yellow
} target_goal_type;

void setup() {
    Serial.begin(9600);

    SETUP_MODULE(Battery);
    SETUP_MODULE(MotorController);
    SETUP_MODULE(Camera);
    SETUP_MODULE(Gyro);
    SETUP_MODULE(LineSensors);
    SETUP_MODULE(Dribbler);
    SETUP_MODULE(Kicker);
    SETUP_MODULE(StartSwitch);
    SETUP_MODULE(LineSensorLed);
    SETUP_MODULE(SwitchLedG);
    SETUP_MODULE(SwitchLedR);

    // Wio で使ってるので。(他のモジュールでbeginしてると思うけど一応)
    I2CManager.beginWire();

    attachInterrupt(LineSensors.getInterruptPin(), interruptHandler, RISING);
    Serial.println("Attach interrupt");

    ToFSensor.init();
    ToFSensor.configureDefault();
    ToFSensor.setScaling(1);
    ToFSensor.setAddress(0x29);
    ToFSensor.setTimeout(100);
    Serial.println("DONE setup ToFSensor");

    // setup 完了通知
    SwitchLedG.ternOn();
    SwitchLedR.ternOn();

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

    // カメラが有効になるのを待つ
    while (true) {
        if (Camera.available()) {
            break;
        }
        Serial.println("Camera not available");
        delay(1000);
    }
    SwitchLedG.ternOff();
    SwitchLedR.ternOn();
}

void loop() {

    if (Battery.isEmergency()) {
        while (true) {
            SwitchLedG.ternOn();
            SwitchLedR.ternOff();
            delay(200);
            SwitchLedG.ternOff();
            SwitchLedR.ternOn();
            delay(200);
        }
    }

    // フィールド状況読み取り
    target_goal_type = GoalType::Blue;
    rotation = Gyro.getRotation();
    ball = Camera.getBallData();
    yellow_goal = Camera.getYellowGoalData();
    blue_goal = Camera.getBlueGoalData();
    ball_front_dist = ToFSensor.readRangeSingleMillimeters();

    // Wio terminal に データ送信
    Wire.beginTransmission(0x11);
    const uint8_t wio_send_data[10] = {
        (uint8_t)(int8_t)ball.position.x, (uint8_t)(int8_t)ball.position.y,
        (uint8_t)(int8_t)yellow_goal.position.x, (uint8_t)(int8_t)yellow_goal.position.y, (uint8_t)(yellow_goal.width / 2),
        (uint8_t)(int8_t)blue_goal.position.x, (uint8_t)(int8_t)blue_goal.position.y, (uint8_t)(blue_goal.width / 2),
        (uint8_t)(Battery.voltage() * 10),
        (uint8_t)ball_front_dist,
    };
    Wire.write(wio_send_data, 10);
    Wire.endTransmission();

    if (StartSwitch.isHigh()) {
        // ロボット停止
        SwitchLedR.ternOn();
        SwitchLedG.ternOff();
        Dribbler.stop();
        LineSensorLed.ternOff();
        MotorController.freeAll();
        delay(50);
        return;
    }

    // スタート
    SwitchLedR.ternOff();
    SwitchLedG.ternOn();
    LineSensorLed.ternOn();

    if (ball.is_exist == false && ball_front_dist == 255) {
        MotorController.stopAll();
        return;
    }

    Serial.println("{");

    Serial.print("  blue_goal pos: ");
    Serial.print(blue_goal.position.x);
    Serial.print(", ");
    Serial.println(blue_goal.position.y);

    Serial.print("  blue_goal exist: ");
    Serial.println(blue_goal.is_exist ? "true" : "false");

    Serial.print("  blue_goal width: ");
    Serial.println(blue_goal.width);

    Serial.println("}");


    // ボールに近いが attacker になる
    attacker();

    // カメラのI2Cをやりすぎないように少し待つ
    delay(10);
}

void attacker() {
    static float pre_ball_dist = 0;

    const float ball_dist = Vector2::norm(ball.position);
    const auto attack_goal = (target_goal_type == GoalType::Blue ? blue_goal : yellow_goal);
    const auto defence_goal = (target_goal_type == GoalType::Blue ? yellow_goal : blue_goal);
    const auto ball_dist_diff = ball_dist - pre_ball_dist;
    pre_ball_dist = ball_dist;

    if (ball_front_dist < 255) {
        Dribbler.start(100);
    } else {
        Dribbler.stop();
    }

    if (ball_front_dist < 255) {

        // ボール保持している時
        if (attack_goal.is_exist == false) {
            // ゴールが見えない時は相手と接触している状態なので、全力で押す
            azimuth = PI / 2;
            MotorController.drive(azimuth, 100, -rotation);
            return;
        }
        // ゴールが見えている
        if (attack_goal.width >= 80 || attack_goal.position.y < 50) {
            Dribbler.stop();
            Kicker.pushFront();
            delay(200);
            Kicker.pullBack();
            MotorController.drive(3 * PI / 2, 50, -rotation); // リバンドに備える(下がるだけなのでazimuthの更新はしなくていい？)
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
    const auto attack_goal = (target_goal_type == GoalType::Blue ? blue_goal : yellow_goal);
    const auto defence_goal = (target_goal_type == GoalType::Blue ? yellow_goal : blue_goal);

    // ボールの方へ横移動
    volatile float azimuth = defence_goal.x > 0 ? 0: PI;
    if (defencd_goal.width >= 80){
        // TODO: width のパラメータ調整
        // ゴールから近いときは前に進む
        // x のみずれても width が変わるので、不規則に動いてくれる？
        azimuth = (azimuth + (1 / 2 * PI)) / 2;
    }else{
        azimuth = (azimuth + (3 / 2 * PI)) / 2;
    }

    MotorController.drive(azimuth, 30, -rotation)

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
