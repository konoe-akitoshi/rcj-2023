#ifndef COMPONENTS_GYRO_HPP
#define COMPONENTS_GYRO_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#include "../../local/Adafruit_BNO055.hpp"
#else
#include "Adafruit_BNO055.h"
#endif

#include "setup_handler.hpp"

// Set the delay between fresh samples
#define BNO055_SAMPLERATE_DELAY_MS (50)

namespace component
{
class Gyro
{
  public:
    explicit Gyro(SetupHandler& handler);
    int GetRotation();
    void SetZeroRoration();
  private:
    // Check I2C device address and correct line below (by default address is 0x29 or 0x28)
    //     ADO = LOW > Address = 0x28
    //     ADO = Open > Address = 0x29
    //   first: id, second: address
    Adafruit_BNO055 bno_ = Adafruit_BNO055(-1, 0x28);
    int offset = 0;
    int ReadRoration_();
};

inline Gyro::Gyro(SetupHandler& handler)  {
    handler.SetCallback([&]() {
        bool stats = bno_.begin();
        if (stats == false) {
            Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        }
        delay(1000); // なんか必要らしい
        bno_.setExtCrystalUse(true);
    });
}

inline int Gyro::GetRotation() {
    int rot = ReadRoration_();
    rot = (rot - offset + 360) % 360;
    if (rot > 180) {
        rot -= 360;
    }
    return float(rot * 100) / 180;
}

inline void Gyro::SetZeroRoration() {
    offset = ReadRoration_();
}

inline int Gyro::ReadRoration_() {
  // Possible vector values can be:
  // - VECTOR_ACCELEROMETER - m/s^2
  // - VECTOR_MAGNETOMETER  - uT
  // - VECTOR_GYROSCOPE     - rad/s
  // - VECTOR_EULER         - degrees
  // - VECTOR_LINEARACCEL   - m/s^2
  // - VECTOR_GRAVITY       - m/s^2
  const imu::Vector<3> euler = bno_.getVector(Adafruit_BNO055::VECTOR_EULER);
  return euler.x();
}
}  // namespace component

#endif
