#include "module/camera_controller.hpp"
#include "module/main_wire.hpp"
#include "module/terminal.hpp"
#include "module/types.hpp"

volatile CameraFieldData field_data;
volatile uint8_t request_id;

#define FILL_REQUEST_RETURN(ret, value, idx1, idx2)  \
    if ((value) < 0) {                               \
        (ret)[(idx1)] = 1 << 7;                      \
    } else {                                         \
        (ret)[(idx1)] = ((value) >> 7) & 0b11111111; \
        (ret)[(idx2)] = ((value) >> 0) & 0b11111111; \
    }

void onReceive(int count) {
    for (int i = 0; i < count; ++i) {
        request_id = MainWire.read();
    }
}

void onRequest() {
    uint8_t ret[7] = {0};
    if (request_id == 1) {
        ret[0] = field_data.ballExist;
        FILL_REQUEST_RETURN(ret, field_data.ballPosition.x, 1, 2);
        FILL_REQUEST_RETURN(ret, field_data.ballPosition.y, 3, 4);
    } else if (request_id == 2) {
        ret[0] = field_data.yellowGoalExist;
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.x, 1, 2);
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.y, 3, 4);
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalWidth, 5, 6);
    } else if (request_id == 3) {
        ret[0] = field_data.blueGoalExist;
        FILL_REQUEST_RETURN(ret, field_data.blueGoalPosition.x, 1, 2);
        FILL_REQUEST_RETURN(ret, field_data.blueGoalPosition.y, 3, 4);
        FILL_REQUEST_RETURN(ret, field_data.blueGoalWidth, 5, 6);
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

    Terminal.setup();
    Serial.println("DONE setup Terminal");

    MainWire.setup();
    MainWire.start(onReceive, onRequest);
    Serial.println("DONE setup MainWire");
}

constexpr Vector2Int fixBySectionSeparate(const Vector2Int& vec) {
    const int section_x = vec.x / 40;
    const int section_y = vec.y / 80;
    int ret_x = 0, ret_y = 0;
    if (section_y == 0) {
        ret_y = 65;
        ret_x = (section_x - 4) * 17 + (17 / 2);
    } else if (section_y == 1) {
        ret_y = 15;
        ret_x = (section_x - 4) * 6 + (6 / 2);
    } else {
        ret_y = 3;
        ret_x = (section_x - 4) * 3 + (3 / 2);
    }
    return {ret_x, ret_y};
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
        data.ballExist ? translate(fixBySectionSeparate(data.ballPosition), rotation, move_x, move_y) : Vector2Int(0, 0),
        data.yellowGoalExist ? translate(fixBySectionSeparate(data.yellowGoalPosition), rotation, move_x, move_y) : Vector2Int(0, 0),
        data.blueGoalExist ? translate(fixBySectionSeparate(data.blueGoalPosition), rotation, move_x, move_y) : Vector2Int(0, 0),
        data.ballExist,
        data.yellowGoalExist,
        data.blueGoalExist,
        data.yellowGoalWidth,
        data.blueGoalWidth);
}

void loop() {
    static module::Image image;
    static CameraFieldData cam[4];

    const int terminalRequest = Terminal.popRequest();

    if (terminalRequest >= 5) {
        const int camera_index = terminalRequest - 5;
        CameraController.scanImage(camera_index, image);
        Terminal.writeImage(image);
        return;
    }

    if (terminalRequest > 0) {
        const int camera_index = terminalRequest - 1;
        CameraController.setWhiteBlance(camera_index);
        Terminal.writeMessage(camera_index + 1);
        return;
    }

    // terminalRequest == 0

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
    for (int i = 0; i < 4; ++i) {
        if (cam[i].yellowGoalExist) {
            sumVector += cam[i].yellowGoalPosition;
            count += 1;
        }
    }
    if (count != 0) {
        field_data.yellowGoalPosition = sumVector / count;
        field_data.yellowGoalExist = true;
    } else {
        field_data.yellowGoalExist = false;
    }

    sumVector = {0, 0};
    count = 0;
    for (int i = 0; i < 4; ++i) {
        if (cam[i].blueGoalExist) {
            sumVector += cam[i].blueGoalPosition;
            count += 1;
        }
    }
    if (count != 0) {
        field_data.blueGoalPosition = sumVector / count;
        field_data.blueGoalExist = true;
    } else {
        field_data.blueGoalExist = false;
    }

    CameraFieldData::dumpToSerial(field_data);
    delay(1000);
}
