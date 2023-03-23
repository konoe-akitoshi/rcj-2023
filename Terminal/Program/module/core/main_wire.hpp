#ifndef MODULE_CORE_MAIN_WIRE_HPP
#define MODULE_CORE_MAIN_WIRE_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#include "deps/wire.hpp"
#else
#include "Arduino.h"
#include "Wire.h"
#endif

namespace module {

class MainWire {
  public:
    explicit constexpr MainWire(const uint8_t address) : ADDRESS_(address) {}

    void setup(void) const {
        Wire1.begin(ADDRESS_);
    }
    void start(void (*on_receive)(int)) const {
        Wire1.onReceive(on_receive);
        Wire1.begin(ADDRESS_);
    }

    void write(const uint8_t data) const {
        Wire1.write(data);
    }

    uint8_t read() const {
        return (uint8_t)Wire1.read();
    }

  private:
    const uint8_t ADDRESS_;
};

}

#endif
