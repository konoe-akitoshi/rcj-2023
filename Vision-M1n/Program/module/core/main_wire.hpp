#ifndef MODULE_CORE_MAIN_WIRE_HPP
#define MODULE_CORE_MAIN_WIRE_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/arduino.hpp"
#include "deps/wire.hpp"
#else
#include "Arduino.h"
#include "Wire.h"
#endif

namespace module
{
class MainWire
{
  public:
    explicit constexpr MainWire(const pin_size_t sda, const pin_size_t scl, const uint8_t address)
        : SDA_(sda), SCL_(scl), ADDRESS_(address) {}

    void setup() const {
        Wire1.setSDA(SDA_);
        Wire1.setSCL(SCL_);
    }

    void start(void (*on_receive)(int), void (*on_request)(void)) const {
        Wire1.onReceive(on_receive);
        Wire1.onRequest(on_request);
        Wire1.begin(ADDRESS_);
    }

    void write(const uint8_t (&data)[4]) const {
        Wire1.write(data, 4);
    }

    uint8_t read() const {
        return (uint8_t)Wire1.read();
    }

  private:
    const pin_size_t SDA_;
    const pin_size_t SCL_;
    const uint8_t ADDRESS_;
};
}  // namespace module

#endif
