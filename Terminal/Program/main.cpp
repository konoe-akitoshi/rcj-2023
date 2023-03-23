#include "module/button.hpp"
#include "module/display.hpp"
#include "module/main_wire.hpp"
#include "module/ui/select_view.hpp"

void home();
void displayBatteryVoltage(); // 1
void displayFieldObjectData(); // 2
void displayToFSensorValue(); // 3

const String HomeSelectItems[3] = {"Battery Voltage", "Field Object Data", "ToF Sensor Value"};
ui::SelectView HomeSelectorView(Display, 3, HomeSelectItems, 8);
bool redraw = true;
int scene = 0; // = Home

int battery_voltage = 0;
int tof_sensor_value = 0;
int ball_data[2] = {0, 0};
int yellow_goal_data[3] = {0, 0, 0};
int blue_goal_data[3] = {0, 0, 0};

long current_time_sec = 0;

void onReceive(int count) {
    if (count == 10) {
        ball_data[0] = (int8_t)MainWire.read();
        ball_data[1] = (int8_t)MainWire.read();
        yellow_goal_data[0] = (int8_t)MainWire.read();
        yellow_goal_data[1] = (int8_t)MainWire.read();
        yellow_goal_data[2] = (int)MainWire.read() * 2;
        blue_goal_data[0] = (int8_t)MainWire.read();
        blue_goal_data[1] = (int8_t)MainWire.read();
        blue_goal_data[2] = (int)MainWire.read() * 2;
        battery_voltage = MainWire.read();
        tof_sensor_value = MainWire.read();
    } else {
        for (int i = 0; i < count; ++i) {
            Wire.read(); // ignore
        }
        Display.drawFlame(TFT_GREEN);
    }
}

void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("Open Serial(9600)");

    Display.setup();
    StickUp.setup();
    StickDown.setup();
    StickPress.setup();
    ButtonLeft.setup();
    MainWire.setup();

    MainWire.start(onReceive);
}

void loop() {
    current_time_sec = millis() / 1000;
    if (scene == 0) {
        home();

    } else if (scene == 1) {
        displayBatteryVoltage();
    } else if (scene == 2) {
        displayFieldObjectData();
    } else if (scene == 3) {
        displayToFSensorValue();
    } else {
        Display.drawFlame(TFT_BLUE);
    }
}

void home() {
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("HOME", 0);
        HomeSelectorView.show();
        redraw = false;
    }

    const int m = current_time_sec / 60;
    const int s = current_time_sec % 60;
    const auto timeText = String::format("%02d:%02d", m, s);
    Display.drawText(timeText, {-65, 45}, 4);

    if (StickUp.isPressed()) {
        HomeSelectorView.prev();
        delay(200);
    }
    if (StickDown.isPressed()) {
        HomeSelectorView.next();
        delay(200);
    }
    if (StickPress.isPressed()) {
        scene = HomeSelectorView.get() + 1;
        redraw = true;
        delay(200);
    }
}

void displayBatteryVoltage() {
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("Battery Voltage", 0);
        redraw = false;
    }

    const auto voltage = String::format("%d.%d V", battery_voltage / 10, battery_voltage % 10);
    Display.drawText(voltage, {-70, 20}, 5);

    if (ButtonLeft.isPressed()) {
        scene = 0;
        redraw = true;
    }
}

void displayFieldObjectData() {
    static const String indent = String(" ");
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("Field Object Data", 0);
        redraw = false;
    }

    Display.drawStringLine(indent + "Ball   : (" + String::format("% 4d", ball_data[0]) + ", " + String::format("% 4d", ball_data[1]) + ")", 3);
    Display.drawStringLine(indent + "Goal Y : (" + String::format("% 4d", yellow_goal_data[0]) + ", " + String::format("% 4d", yellow_goal_data[1]) + ")", 4);
    Display.drawStringLine(indent + "Goal B : (" + String::format("% 4d", blue_goal_data[0]) + ", " + String::format("% 4d", blue_goal_data[1]) + ")", 5);

    Display.drawStringLine(indent + "Goal Width Y : " + String::format("% 3d", yellow_goal_data[2]), 7);
    Display.drawStringLine(indent + "Goal Width B : " + String::format("% 3d", blue_goal_data[2]), 8);

    if (ButtonLeft.isPressed()) {
        scene = 0;
        redraw = true;
    }
}

void displayToFSensorValue() {
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("ToF Sensor Value", 0);
        redraw = false;
    }

    const auto voltage = String::format("%03d", tof_sensor_value);
    Display.drawText(voltage, {-50, 20}, 5);

    if (ButtonLeft.isPressed()) {
        scene = 0;
        redraw = true;
    }
}
