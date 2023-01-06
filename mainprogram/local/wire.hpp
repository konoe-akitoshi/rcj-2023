// Wire.h (https://github.com/arduino/ArduinoCore-arc32/blob/master/libraries/Wire/src/Wire.h)
#if defined(LOCAL_INCLUDE) && !defined(LOCAL_WIRE_HPP)
#define LOCAL_WIRE_HPP

#ifdef LOCAL_INCLUDE
#include "arduino_deps.hpp"
#endif

#define BUFFER_LENGTH 32
#define I2C_SPEED_SLOW 1
#define I2C_SPEED_FAST 2

class TwoWire
{
  public:
    TwoWire();
    void begin(void);
    void begin(int speed);
    void setClock(long speed);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(int, int);
    uint8_t requestFrom(int, int, int);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *, size_t);
    virtual int available(void);
    virtual int read(void);
    virtual int peek(void);
    virtual void flush(void);

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
};

extern TwoWire Wire;

#endif
