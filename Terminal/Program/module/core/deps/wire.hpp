#ifndef MODULE_CORE_DEPS_WIRE_HPP
#define MODULE_CORE_DEPS_WIRE_HPP

#include "arduino.hpp"

#define BUFFER_LENGTH 32
#define I2C_SPEED_SLOW 1
#define I2C_SPEED_FAST 2

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

#ifndef WIRE_BUFFER_SIZE
#define WIRE_BUFFER_SIZE 256
#endif

class TwoWire {
public:
    // Start as Master
    void begin();
    // Start as Slave
    void begin(uint8_t address);
    // Shut down the I2C interface
    void end();

    // Select IO pins to use.  Call before ::begin()
    bool setSDA(uint8_t sda);
    bool setSCL(uint8_t scl);
    void setClock(uint32_t freqHz);
    void beginTransmission(uint8_t);
    uint8_t endTransmission(bool stopBit);
    uint8_t endTransmission(void);
    size_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
    size_t requestFrom(uint8_t address, size_t quantity);
    size_t write(uint8_t data);
    size_t write(const uint8_t * data, size_t quantity);
    int available(void);
    int read(void);
    int peek(void);
    void flush(void);
    void onReceive(void(*)(int));
    void onRequest(void(*)(void));
    inline size_t write(unsigned long n);
    inline size_t write(long n);
    inline size_t write(unsigned int n);
    inline size_t write(int n);
    // using Print::write;
    // IRQ callback
    void onIRQ();
};

extern TwoWire Wire;
extern TwoWire Wire1;

#endif
