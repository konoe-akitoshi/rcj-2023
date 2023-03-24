#include "module/motor_controller.hpp"

class SerialReader
{
  public:
    explicit constexpr SerialReader() {}
    float readFloat() const {
        char buf[40] = {0};
        wait_();
        int size = Serial.available();
        for (int i = 0; i < size; ++i) {
            buf[i] = (char)Serial.read();
        }
        int dot_idx = size - 1;
        for (int i = 0; i < size; ++i) {
            if (buf[i] == '.') {
                dot_idx = i;
                break;
            }
        }
        float ret = 0;
        float e = pow(10, dot_idx - 1);
        int now = 0;
        char c = buf[now];
        const bool f = (buf[now] == '-') && (c = buf[++now], e /= 10);
        while (now + 1 < size) {
            if (c != '.') {
                ret += (c - '0') * e;
                e /= 10;
            }
            c = buf[++now];
        }
        return f ? -ret : ret;
    }

  private:
    void wait_() const {
        while (Serial.available() == 0) {
            ;
        }
        delay(50);
    }
};

constexpr SerialReader Reader;

void setup() {
    do {
        Serial.begin(9600);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    MotorController.setup();
    MotorController.stopAll();
    Serial.println("DONE: setup MotorController");
}

void loop() {
    const float azimuth = Reader.readFloat();
    Serial.println(azimuth);
    MotorController.drive(azimuth, 30, 0);
}