#include "module/button.hpp"
#include "module/display.hpp"
#include "module/main_wire.hpp"
#include "module/ui/select_view.hpp"

void home();
void displayBatteryVoltage();   // 1
void displayFieldObjectData();  // 2
void displayToFSensorValue();   // 3
void displayGyroViewer();       // 4
void blackOut();                // 5

const String HomeSelectItems[4] = {"Battery Voltage", "Field Object Data", "ToF Sensor Value", "Gyro Viewer"};
ui::SelectView HomeSelectorView(Display, 4, HomeSelectItems, 7);
bool redraw = true;
int scene = 0;  // = Home

volatile int battery_voltage = 0;
volatile int machine_rotation = 0;

volatile int ball_data[3] = {0, 0, 0};            // [is_exist, pos_x, pos_y]
volatile int yellow_goal_data[4] = {0, 0, 0, 0};  // [is_exist, pos_x, pos_y, width]
volatile int blue_goal_data[4] = {0, 0, 0, 0};    // [is_exist, pos_x, pos_y, width]
volatile int tof_sensor_value = 0;

void onReceive(int count) {
    if (count == 1) {
        if (MainWire.read() != 0) {
            Display.backlightTurnOn();
            Display.drawFlame(TFT_RED);
        }
    } else if (count == 2) {
        battery_voltage = MainWire.read();
        machine_rotation = (int8_t)MainWire.read();
    } else if (count == 10) {
        const uint8_t exist_flag = MainWire.read();
        ball_data[0] = (exist_flag & 0b001);
        ball_data[1] = (int8_t)MainWire.read();
        ball_data[2] = (int8_t)MainWire.read();
        yellow_goal_data[0] = (exist_flag & 0b010);
        yellow_goal_data[1] = (int8_t)MainWire.read();
        yellow_goal_data[2] = (int8_t)MainWire.read();
        yellow_goal_data[3] = (int)MainWire.read() * 2;
        blue_goal_data[0] = (exist_flag & 0b100);
        blue_goal_data[1] = (int8_t)MainWire.read();
        blue_goal_data[2] = (int8_t)MainWire.read();
        blue_goal_data[3] = (int)MainWire.read() * 2;
        tof_sensor_value = MainWire.read();
    } else {
        for (int i = 0; i < count; ++i) {
            Wire.read();  // ignore
        }
        Display.backlightTurnOn();
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
    StickRight.setup();
    StickLeft.setup();
    StickPress.setup();
    ButtonLeft.setup();
    ButtonCenter.setup();
    ButtonRight.setup();
    MainWire.setup();

    MainWire.start(onReceive);
}

void loop() {
    if (scene == 0) {
        home();
    } else if (scene == 1) {
        displayBatteryVoltage();
    } else if (scene == 2) {
        displayFieldObjectData();
    } else if (scene == 3) {
        displayToFSensorValue();
    } else if (scene == 4) {
        displayGyroViewer();
    } else if (scene == 5) {
        blackOut();
    } else {
        Display.backlightTurnOn();
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

    const int current_time_sec = millis() / 1000;
    const int m = current_time_sec / 60;
    const int s = current_time_sec % 60;
    const auto timeText = String::format("%02d:%02d", m, s);
    Display.drawText(timeText, {-65, 50}, 4);

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
    }
    if (ButtonCenter.isPressed()) {
        scene = 5;
        redraw = true;
    }
    if (ButtonRight.isPressed()) {
        Display.drawFlame(Display.BACKGROUND_COLOR);
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

String format4(const int value) {
    return String::format("%04d", value);
}

String format3(const int value) {
    return String::format("%03d", value);
}

void displayFieldObjectData() {
    static const String indent = String(" ");
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("Field Object Data", 0);
        redraw = false;
    }

    Display.drawStringLine(indent + "Ball   : (" + (ball_data[0] ? (format4(ball_data[1]) + ", " + format4(ball_data[2])) : "____, ____") + ")", 3);
    Display.drawStringLine(indent + "Goal Y : (" + (yellow_goal_data[0] ? (format4(yellow_goal_data[1]) + ", " + format4(yellow_goal_data[2])) : "____, ____") + ")", 4);
    Display.drawStringLine(indent + "Goal B : (" + (blue_goal_data[0] ? (format4(blue_goal_data[1]) + ", " + format4(blue_goal_data[2])) : "____, ____") + ")", 5);

    Display.drawStringLine(indent + "Goal Width Y : " + (yellow_goal_data[0] ? format3(yellow_goal_data[3]) : "___"), 7);
    Display.drawStringLine(indent + "Goal Width B : " + (blue_goal_data[0] ? format3(blue_goal_data[3]) : "___"), 8);

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

void displayGyroViewer() {
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        Display.drawStringLine("Gyro Viewer", 0);
        redraw = false;
    }

    Display.fillRect({-70, 70}, 140, 140, Display.BACKGROUND_COLOR);
    Display.drawRect({-5, 5}, 10, 10, TFT_LIGHTGREY);
    Display.drawRect({-10, 10}, 20, 20, TFT_LIGHTGREY);
    const float theta = (machine_rotation * PI / 100) + (PI / 2);
    const int x = 60 * cos(theta);
    const int y = 60 * sin(theta);
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            Display.drawLine({i, j}, {x + i, y + j});
        }
    }

    delay(200);

    if (ButtonLeft.isPressed()) {
        scene = 0;
        redraw = true;
    }
}

void blackOut() {
    if (redraw) {
        Display.fillScreen(Display.BACKGROUND_COLOR);
        delay(200);
        Display.backlightTurnOff();
    }
    if (ButtonLeft.isPressed() || ButtonCenter.isPressed() || ButtonRight.isPressed()
        || StickUp.isPressed() || StickRight.isPressed() || StickDown.isPressed() || StickLeft.isPressed() || StickPress.isPressed()) {
        scene = 0;
        redraw = true;
        Display.backlightTurnOn();
    }
}
