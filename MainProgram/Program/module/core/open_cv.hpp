#ifndef MODULE_CORE_OPEN_CV_HPP
#define MODULE_CORE_OPEN_CV_HPP

#include <array>
#include "../vector2.hpp"

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#else
#include <Arduino.h>
#endif

namespace module
{
class OpenCV
{
  public:
    /**
     * @param speed transfer speed rate (in bits per second).
     */
    explicit constexpr OpenCV(const int speed) : SPEED_(speed), raw_() {
    }

    void setup(void) const {
        do {
            Serial2.begin(SPEED_);
            delay(100);
        } while (!Serial2);
    }

    /**
     * Wait for data from Vison-OpenMV.
     */
    void waitData(void) {
        while (true) {
            raw_[0] = getDataUnit_();
            if (raw_[0] == 254) {
                break;
            }
        }
        for (uint8_t i = 1; i < DATA_SIZE_; ++i) {
            raw_[i] = getDataUnit_();
        }
    }

    /**
     * Get the ball count.
     *
     * @return true if the ball exists.
     */
    int getBallExistence(void) const {
        return raw_[1];
    }

    /**
     * Get the yellow goal count.
     *
     * @return true if the yellow goal exists.
     */
    int getYellowGoalExistence(void) const {
        return raw_[9];
    }

    /**
     * Get the blue goal count.
     *
     * @return true if the blue goal exists.
     */
    int getBlueGoalExistence(void) const {
        return raw_[17];
    }

    /**
     * Get the ball position.
     *
     * @return the coordinate of the ball.
     */
    Vector2 getBallPosition(void) const {
        const int x = raw_[2] + raw_[3];
        const int y = raw_[4] + raw_[5];
        return {x, y};
    }

    /**
     * Get the yellow goal position.
     *
     * @return the coordinate of yellow goal.
     */
    Vector2 getYellowGoalPosition(void) const {
        const int x = raw_[10] + raw_[11];
        const int y = raw_[12] + raw_[13];
        return {x, y};
    }

    /**
     * Get the blue goal position.
     *
     * @return the coordinate of blue goal.
     */
    Vector2 getBlueGoalPosition(void) const {
        const int x = raw_[18] + raw_[19];
        const int y = raw_[20] + raw_[21];
        return {x, y};
    }

    /**
     * Get the yellow goal width.
     *
     * @return the width of yellow goal.
     */
    int getYellowGoalWidth(void) const {
        return raw_[14] + raw_[15];
    }

    /**
     * Get the blue goal width.
     *
     * @return the width of blue goal.
     */
    int getBlueGoalWidth(void) const {
        return raw_[22] + raw_[23];
    }

  private:
    static constexpr uint8_t DATA_SIZE_ = 24;
    const int SPEED_;
    std::array<int, DATA_SIZE_> raw_;

    inline int getDataUnit_() const {
        while (Serial2.available() == 0) {
            ;  // wait for serial data
        }
        return Serial2.read();
    }
};
}  // namespace module

#endif
