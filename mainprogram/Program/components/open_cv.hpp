#ifndef COMPONENTS_OPEN_VM_HPP
#define COMPONENTS_OPEN_VM_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

#include <array>
#include "../types/vector2.hpp"
#include "setup_handler.hpp"

namespace component
{
class OpenCV
{
  public:
    /**
     * @param speed transfer speed rate (in bits per second).
     */
    explicit OpenCV(SetupHandler& handler, const int speed);

    /**
     * Wait for data from Vison-OpenMV.
     */
    void WaitData();

    /**
     * Get the ball count.
     *
     * @return true if the ball exists.
     */
    int GetBallExistence() const;

    /**
     * Get the yellow goal count.
     *
     * @return true if the yellow goal exists.
     */
    int GetYellowGoalExistence() const;

    /**
     * Get the blue goal count.
     *
     * @return true if the blue goal exists.
     */
    int GetBlueGoalExistence() const;

    /**
     * Get the ball position.
     *
     * @return the coordinate of the ball.
     */
    Vector2 GetBallPosition() const;

    /**
     * Get the yellow goal position.
     *
     * @return the coordinate of yellow goal.
     */
    Vector2 GetYellowGoalPosition() const;

    /**
     * Get the blue goal position.
     *
     * @return the coordinate of blue goal.
     */
    Vector2 GetBlueGoalPosition() const;

    /**
     * Get the yellow goal width.
     *
     * @return the width of yellow goal.
     */
    int GetYellowGoalWidth() const;

    /**
     * Get the blue goal width.
     *
     * @return the width of blue goal.
     */
    int GetBlueGoalWidth() const;

  private:
    static constexpr int DATA_SIZE_ = 24;
    std::array<int, DATA_SIZE_> raw_;
    inline int GetDataUnit_() const {
        while (Serial2.available() == 0) {
            ;  // wait for serial data
        }
        return Serial2.read();
    }
};

inline OpenCV::OpenCV(SetupHandler& handler, const int speed) {
    handler.SetCallback([speed]() {
        Serial2.begin(speed);
    });
}

inline void OpenCV::WaitData() {
    while (true) {
        raw_[0] = GetDataUnit_();
        if (raw_[0] == 254) {
            break;
        }
    }
    for (int i = 1; i < DATA_SIZE_; ++i) {
        raw_[i] = GetDataUnit_();
    }
}

inline int OpenCV::GetBallExistence() const {
    return raw_[1];
}

inline int OpenCV::GetYellowGoalExistence() const {
    return raw_[9];
}

inline int OpenCV::GetBlueGoalExistence() const {
    return raw_[17];
}

inline Vector2 OpenCV::GetBallPosition() const {
    const int x = raw_[2] + raw_[3];
    const int y = raw_[4] + raw_[5];
    return {x, y};
}

inline Vector2 OpenCV::GetYellowGoalPosition() const {
    const int x = raw_[10] + raw_[11];
    const int y = raw_[12] + raw_[13];
    return {x, y};
}

inline Vector2 OpenCV::GetBlueGoalPosition() const {
    const int x = raw_[18] + raw_[19];
    const int y = raw_[20] + raw_[21];
    return {x, y};
}

inline int OpenCV::GetYellowGoalWidth() const {
    return raw_[14] + raw_[15];
}

inline int OpenCV::GetBlueGoalWidth() const {
    return raw_[22] + raw_[23];
}
}  // namespace component

#endif
