#include "module/camera_controller.hpp"
#include "module/main_wire.hpp"
#include "module/types.hpp"

volatile CameraFieldData field_data;
volatile uint8_t request_id = 0;

void onReceive(int count) {
    for (int i = 0; i < count; ++i) {
        request_id = MainWire.read();
    }
}

void onRequest() {
    uint8_t ret[4] = {0};
    if (request_id == 1) {
        ret[0] = (field_data.ballExist ? 1 : 0);
        ret[1] = field_data.ballPosition.x;
        ret[2] = field_data.ballPosition.y;
    } else if (request_id == 2) {
        ret[0] = (field_data.yellowGoalExist ? 1 : 0);
        ret[1] = field_data.yellowGoalPosition.x;
        ret[2] = field_data.yellowGoalPosition.y;
        ret[3] = field_data.yellowGoalWidth;
    } else if (request_id == 3) {
        ret[0] = (field_data.blueGoalExist ? 1 : 0);
        ret[1] = field_data.blueGoalPosition.x;
        ret[2] = field_data.blueGoalPosition.y;
        ret[3] = field_data.blueGoalWidth;
    }
    MainWire.write(ret);
}

void setup() {
    Serial.begin(9600);
    delay(1000);
    if (Serial) {
        Serial.println("DONE open Serial(9600)");
    } else {
        Serial.println("WARN Serial(9600) does not open");
    }

    CameraController.setup();
    Serial.println("DONE setup CameraController");

    // NOTE: MainWire must be last because the start of I2C connection is used
    //       in MainProgram for check whether Camera has been setup.
    MainWire.setup();
    MainWire.start(onReceive, onRequest);
    Serial.println("DONE setup MainWire");
}

constexpr Vector2Int translate(const Vector2Int& vec, const float theta, const float tx, const float ty) {
    // / \   /              \  / \
    // |X|   |cosθ  -sinθ  Tx| |x|
    // |Y| = |sinθ   cosθ  Ty| |y|
    // |1|   |  0     0    1 | |1|
    // \_/   \______________/  \_/
    const int x = (vec.x * cos(theta)) - (vec.y * sin(theta)) + tx;
    const int y = (vec.x * sin(theta)) + (vec.y * cos(theta)) + ty;
    return {x, y};
}

constexpr CameraFieldData fixFieldData(const CameraFieldData& data, const float rotation, const float move_x, const float move_y) {
    return CameraFieldData(
        data.ballExist ? translate(data.ballPosition, rotation, move_x, move_y) : Vector2Int(0, 0),
        data.yellowGoalExist ? translate(data.yellowGoalPosition, rotation, move_x, move_y) : Vector2Int(0, 0),
        data.blueGoalExist ? translate(data.blueGoalPosition, rotation, move_x, move_y) : Vector2Int(0, 0),
        data.ballExist,
        data.yellowGoalExist,
        data.blueGoalExist,
        data.yellowGoalWidth,
        data.blueGoalWidth);
}

void loop() {
    static module::Image image;
    static CameraFieldData cam[4];

    cam[0] = fixFieldData(CameraController.getFieldData(0), -1 * PI / 4, 7, -1);
    cam[1] = fixFieldData(CameraController.getFieldData(1), -3 * PI / 4, 7, -16);
    cam[2] = fixFieldData(CameraController.getFieldData(2), -5 * PI / 4, -7, -16);
    cam[3] = fixFieldData(CameraController.getFieldData(3), -7 * PI / 4, -7, -1);

    Vector2Int sumVector = {0, 0};
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (cam[i].ballExist) {
            sumVector += cam[i].ballPosition;
            count += 1;
        }
    }
    if (count != 0) {
        field_data.ballPosition = sumVector / count;
        field_data.ballExist = true;
    } else {
        field_data.ballExist = false;
    }

    sumVector = {0, 0};
    count = 0;
    int sumWidth = 0;
    for (int i = 0; i < 4; ++i) {
        if (cam[i].yellowGoalExist) {
            sumVector += cam[i].yellowGoalPosition * cam[i].yellowGoalWidth;
            sumWidth += cam[i].yellowGoalWidth;
            count += 1;
        }
    }
    if (count != 0) {
        field_data.yellowGoalPosition = sumVector / sumWidth;
        field_data.yellowGoalWidth = sumWidth / count;
        field_data.yellowGoalExist = true;
    } else {
        field_data.yellowGoalExist = false;
    }

    sumVector = {0, 0};
    count = 0;
    sumWidth = 0;
    for (int i = 0; i < 4; ++i) {
        if (cam[i].blueGoalExist) {
            sumVector += cam[i].blueGoalPosition * cam[i].blueGoalWidth;
            sumWidth += cam[i].blueGoalWidth;
            count += 1;
        }
    }
    if (count != 0) {
        field_data.blueGoalPosition = sumVector / sumWidth;
        field_data.blueGoalWidth = sumWidth / count;
        field_data.blueGoalExist = true;
    } else {
        field_data.blueGoalExist = false;
    }

    // CameraFieldData::dumpToSerial(field_data);
    delay(30);
}
