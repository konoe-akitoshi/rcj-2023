#if defined(LOCAL_INCLUDE) && !defined(LOCAL_SERVO_HPP)
#define LOCAL_SERVO_HPP

#include "arduino_deps.hpp"

// Servo.h (https://github.com/arduino-libraries/Servo/blob/master/src/Servo.h)
class Servo
{
  public:
    Servo();

    uint8_t attach(int pin);
    uint8_t attach(int pin, int min, int max);
    bool attached();
    void detach();
    void write(int angle);
    void writeMicroseconds(int us);
    int read();
};

#endif
