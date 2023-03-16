#include "module/camera_controller.hpp"
#include "module/types.hpp"

constexpr int WIRE1_SDA = 6;
constexpr int WIRE1_SCL = 7;
constexpr int WIRE1_ADDRESS = 0x10;

volatile CameraFieldData field_data;

void onRequest() {
    // TODO: field_data いじって配列にして返す
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

    Wire1.setSDA(WIRE1_SDA);
    Wire1.setSCL(WIRE1_SCL);
    Wire1.onRequest(onRequest);
    Wire1.begin(WIRE1_ADDRESS);
    Serial.println("DONE setup Wire1");
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
