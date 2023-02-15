#ifndef MODULE_LINE_SENSOR_HPP
#define MODULE_LINE_SENSOR_HPP

#include <array>
#include "core/digital_reader.hpp"
#include "_pin.hpp"

// TODO: クラスにする。
constexpr std::array<module::DigitalReader, 5> LineSensors = {
    module::DigitalReader(raspberry_pi_pico::PIN21_GP16, INPUT_PULLUP),
    module::DigitalReader(raspberry_pi_pico::PIN22_GP17, INPUT_PULLUP),
    module::DigitalReader(raspberry_pi_pico::PIN24_GP18, INPUT_PULLUP),
    module::DigitalReader(raspberry_pi_pico::PIN25_GP19, INPUT_PULLUP),
    module::DigitalReader(raspberry_pi_pico::PIN26_GP20, INPUT_PULLUP)
};

inline void LineSensors_Seup() {
    for (int i = 0; i < 5; ++i) {
        LineSensors[i].Setup();
    }
}

#endif
