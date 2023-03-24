#ifndef MODULE_CORE_I2C_MANAGER_HPP
#define MODULE_CORE_I2C_MANAGER_HPP

#ifdef WITHOUT_ARDUINO_ENVIRONMENT
#include "deps/Adafruit_PCF8574.hpp"
#include "deps/arduino.hpp"
#include "deps/wire.hpp"
#else
#include "Adafruit_PCF8574.h"
#include "Arduino.h"
#include "Wire.h"
#endif

namespace module
{
class I2CManager
{
  public:
    explicit constexpr I2CManager(
        const pin_size_t wire_sda, const pin_size_t wire_scl, const uint32_t wire_clock,
        const pin_size_t wire1_sda, const pin_size_t wire1_scl,
        Adafruit_PCF8574& pcf8574, const uint8_t pcf8574_address)
        : WIRE_SDA_(wire_sda), WIRE_SCL_(wire_scl), WIRE_CLOCK_(wire_clock),
          WIRE1_SDA_(wire1_sda), WIRE1_SCL_(wire1_scl),
          pcf8574_(pcf8574), PCF8574_ADDRESS_(pcf8574_address) {
    }

    void beginWire() const {
        static bool enabled = false;
        if (enabled) {
            return;
        }
        enabled = true;
        Wire.setSDA(WIRE_SDA_);
        Wire.setSCL(WIRE_SCL_);
        Wire.setClock(WIRE_CLOCK_);
        Wire.begin();
    }

    void beginWire1() const {
        static bool enabled = false;
        if (enabled) {
            return;
        }
        enabled = true;
        Wire1.setSDA(WIRE1_SDA_);
        Wire1.setSCL(WIRE1_SCL_);
        Wire1.begin();
    }

    void beginPCF8574() const {
        static bool enabled = false;
        if (enabled) {
            return;
        }
        enabled = true;

        beginWire1();
        bool pcf8574_status;
        do {
            pcf8574_status = pcf8574_.begin(PCF8574_ADDRESS_, &Wire1);
            if (pcf8574_status == false) {
                Serial.println("ERROR not find PCF8574");
                delay(500);
            }
        } while (!pcf8574_status);
        Serial.println("DONE setup PCF8574");
    }

    void pcf8574_pinMode(const uint8_t pin, const bool value) const {
        pcf8574_.pinMode(pin, value);
    }

    void pcf8574_digitalWrite(const uint8_t pin, const bool value) const {
        pcf8574_.digitalWrite(pin, value);
    }

  private:
    const pin_size_t WIRE_SDA_;
    const pin_size_t WIRE_SCL_;
    const uint32_t WIRE_CLOCK_;
    const pin_size_t WIRE1_SDA_;
    const pin_size_t WIRE1_SCL_;
    Adafruit_PCF8574& pcf8574_;
    const uint8_t PCF8574_ADDRESS_;
};
}  // namespace module

#endif
