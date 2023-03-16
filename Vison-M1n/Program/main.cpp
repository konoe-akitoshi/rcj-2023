#include "module/camera_controller.hpp"
#include "module/main_wire.hpp"
#include "module/types.hpp"

volatile CameraFieldData field_data;
#define FILL_REQUEST_RETURN(ret, value, idx1, idx2)  \
    if ((value) < 0) {                               \
        (ret)[(idx1)] = 1 << 7;                      \
    } else {                                         \
        (ret)[(idx1)] = ((value) >> 7) & 0b11111111; \
        (ret)[(idx2)] = ((value) >> 0) & 0b11111111; \
    }

void onRequest() {
    uint8_t ret[16] = {0};
    FILL_REQUEST_RETURN(ret, field_data.ballPosition.x, 0, 1);
    FILL_REQUEST_RETURN(ret, field_data.ballPosition.y, 2, 3);
    FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.x, 4, 5);
    FILL_REQUEST_RETURN(ret, field_data.yellowGoalPosition.y, 6, 7);
    FILL_REQUEST_RETURN(ret, field_data.yellowGoalWidth, 8, 9);
    FILL_REQUEST_RETURN(ret, field_data.blueGoalPosition.x, 10, 11);
    FILL_REQUEST_RETURN(ret, field_data.blueGoalPosition.y, 12, 13);
    FILL_REQUEST_RETURN(ret, field_data.blueGoalWidth, 14, 15);
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

    MainWire.setup();
    MainWire.start(onRequest);
    Serial.println("DONE setup MainWire");
}

CameraFieldData fixFiledData0(const CameraFieldData& data) {
    // TODO :impl
}

CameraFieldData fixFiledData1(const CameraFieldData& data) {
    // TODO :impl
}

CameraFieldData fixFiledData2(const CameraFieldData& data) {
    // TODO :impl
}

CameraFieldData fixFiledData3(const CameraFieldData& data) {
    // TODO :impl
}

void loop() {
    const auto field_data0 = fixFiledData0(CameraController.getFieldData(0));
    const auto field_data1 = fixFiledData1(CameraController.getFieldData(1));
    const auto field_data2 = fixFiledData2(CameraController.getFieldData(2));
    const auto field_data3 = fixFiledData3(CameraController.getFieldData(3));

    // TODO: impl marge field_data* to field_data
}
