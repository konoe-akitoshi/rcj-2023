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
    // TODO: impl ret[0] (existance data)
    uint8_t ret[7] = {0};
    if (request_id == 1) {
        FILL_REQUEST_RETURN(ret, field_data.ballPosition.x, 1, 2);
        FILL_REQUEST_RETURN(ret, field_data.ballPosition.y, 3, 4);
    } else if (request_id == 2) {
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.x, 1, 2);
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.y, 3, 4);
        FILL_REQUEST_RETURN(ret, field_data.yellowGoalWidth, 5, 6);
    } else if (request_id == 3) {
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

CameraFieldData fixFiledData0(const CameraFieldData& data) {
    // TODO :impl
    if (data.ballPosition.x != -1) {
        return CameraFieldData(
            {1, 1}, {0, 0}, {0, 0},
            0, 0);
    }
    return data;
}

CameraFieldData fixFiledData1(const CameraFieldData& data) {
    // TODO :impl
    if (data.ballPosition.x != -1) {
        return CameraFieldData(
            {1, -1}, {0, 0}, {0, 0},
            0, 0);
    }
    return data;
}

CameraFieldData fixFiledData2(const CameraFieldData& data) {
    // TODO :impl
    if (data.ballPosition.x != -1) {
        return CameraFieldData(
            {-1, -1}, {0, 0}, {0, 0},
            0, 0);
    }
    return data;
}

CameraFieldData fixFiledData3(const CameraFieldData& data) {
    // TODO :impl
    if (data.ballPosition.x != -1) {
        return CameraFieldData(
            {-1, 1}, {0, 0}, {0, 0},
            0, 0);
    }
    return data;
}

void loop() {
    static module::Image image;

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

    const auto cam0 = fixFiledData0(CameraController.getFieldData(0));
    const auto cam1 = fixFiledData1(CameraController.getFieldData(1));
    const auto cam2 = fixFiledData2(CameraController.getFieldData(2));
    const auto cam3 = fixFiledData3(CameraController.getFieldData(3));

    field_data.ballPosition.x = (cam0.ballPosition.x + cam1.ballPosition.x + cam2.ballPosition.x + cam3.ballPosition.x) / 4;
    field_data.ballPosition.y = (cam0.ballPosition.y + cam1.ballPosition.y + cam2.ballPosition.y + cam3.ballPosition.y) / 4;

    field_data.yellowGoalPosition.x = (cam0.yellowGoalPosition.x + cam1.yellowGoalPosition.x + cam2.yellowGoalPosition.x + cam3.yellowGoalPosition.x) / 4;
    field_data.yellowGoalPosition.y = (cam0.yellowGoalPosition.y + cam1.yellowGoalPosition.y + cam2.yellowGoalPosition.y + cam3.yellowGoalPosition.y) / 4;

    field_data.blueGoalPosition.x = (cam0.blueGoalPosition.x + cam1.blueGoalPosition.x + cam2.blueGoalPosition.x + cam3.blueGoalPosition.x) / 4;
    field_data.blueGoalPosition.y = (cam0.blueGoalPosition.y + cam1.blueGoalPosition.y + cam2.blueGoalPosition.y + cam3.blueGoalPosition.y) / 4;

    field_data.yellowGoalWidth = (cam0.yellowGoalWidth + cam1.yellowGoalWidth + cam2.yellowGoalWidth + cam3.yellowGoalWidth) / 4;
    field_data.blueGoalWidth = (cam0.blueGoalWidth + cam1.blueGoalWidth + cam2.blueGoalWidth + cam3.blueGoalWidth) / 4;

    Serial.println(field_data.ballPosition.x);
}
