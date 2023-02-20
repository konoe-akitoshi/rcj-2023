#ifndef MODULE_CORE_DEPS_ARDUINO_HPP
#define MODULE_CORE_DEPS_ARDUINO_HPP

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define RISING 4

#ifdef EXTENDED_PIN_MODE
// Platforms who wnat to declare more than 256 pins need to define EXTENDED_PIN_MODE globally
typedef uint32_t pin_size_t;
#else
typedef uint8_t pin_size_t;
#endif

#define PI (3.1415926535897932384626433832795)
#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/* IO */
extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t val);
extern int digitalRead(uint8_t pin);
extern void analogWrite(uint8_t pin, uint32_t val);
extern uint32_t analogRead(uint32_t ulPin);
extern void analogWriteFreq(uint32_t freq);

void attachInterrupt(uint32_t pin, void (*callback)(void), uint32_t mode);
void detachInterrupt(uint32_t pin);

extern void delay(uint32_t dwMs);
extern uint32_t millis();

/* class */
// 存在しないクラス、適当に作った。
class SerialClass
{
  public:
    SerialClass();
    void begin(unsigned long baud);
    void begin(unsigned long baud, uint16_t config);
    void end();
    int read();
    int available();
    void flush();
    size_t write(uint8_t c);
    size_t write(const uint8_t *p, size_t len);
    operator bool();
    void print(const char* msg);
    void print(const int msg);
    void println(const char* msg);
    void println(const int msg);
};
extern SerialClass Serial;
extern SerialClass Serial1;
extern SerialClass Serial2;
extern SerialClass Serial3;

#endif
