#ifndef LOCAL_ARDUINO_DEPS_H
#define LOCAL_ARDUINO_DEPS_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define RISING 4

#define LED_BUILTIN 13

#define PI 3.1415926535897932384626433832795
#define abs(x) ((x) > 0 ? (x) : -(x))

/* IO */
extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t val);
extern int digitalRead(uint8_t pin);
extern void analogWrite(uint8_t pin, uint32_t val);
extern uint32_t analogRead(uint32_t ulPin);
extern void analogWriteFrequency(uint8_t pin, uint32_t freq);

void attachInterrupt(uint32_t pin, void (*callback)(void), uint32_t mode);
void detachInterrupt(uint32_t pin);

extern void delay(uint32_t dwMs);

/* class */
// 存在しないクラス、適当に作った。
class SerialClass
{
   public:
    void begin(const uint32_t speed);
    int available(void);
    int read(void);
    size_t write(const uint8_t c);
    size_t write(const char *str);
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
