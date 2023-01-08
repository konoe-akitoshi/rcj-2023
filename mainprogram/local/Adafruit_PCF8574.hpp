#if defined(LOCAL_INCLUDE) && !defined(LOCAL_ADAFRUT_PCF8575_HPP)
#define LOCAL_ADAFRUT_PCF8575_HPP

#define PCF8574_I2CADDR_DEFAULT 0x20 ///< PCF8574 default I2C address

#include "arduino_deps.hpp"
#include "wire.hpp"

class Adafruit_PCF8574 {
public:
  Adafruit_PCF8574();
  bool begin(uint8_t i2c_addr = PCF8574_I2CADDR_DEFAULT, TwoWire *wire = &Wire);
  bool digitalWriteByte(uint8_t d);
  bool digitalWrite(uint8_t pinnum, bool val);
  uint8_t digitalReadByte(void);
  bool digitalRead(uint8_t pinnum);
  bool pinMode(uint8_t pinnum, uint8_t val);
};

#endif
