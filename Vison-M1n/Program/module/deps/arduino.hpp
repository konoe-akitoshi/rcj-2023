#ifndef MODULE_CORE_DEPS_ARDUINO_HPP
#define MODULE_CORE_DEPS_ARDUINO_HPP

#include <math.h>
#include <stdint.h>
#include <stdio.h>

void yield(void);

typedef enum {
  LOW     = 0,
  HIGH    = 1,
  CHANGE  = 2,
  FALLING = 3,
  RISING  = 4,
} PinStatus;

typedef enum {
  INPUT            = 0x0,
  OUTPUT           = 0x1,
  INPUT_PULLUP     = 0x2,
  INPUT_PULLDOWN   = 0x3,
  OUTPUT_OPENDRAIN = 0x4,
} PinMode;

typedef enum {
  LSBFIRST = 0,
  MSBFIRST = 1,
} BitOrder;

#define PI          3.1415926535897932384626433832795
#define HALF_PI     1.5707963267948966192313216916398
#define TWO_PI      6.283185307179586476925286766559
#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG  57.295779513082320876798154814105
#define EULER       2.718281828459045235360287471352

#define SERIAL      0x0
#define DISPLAY     0x1

/* TODO: request for removal */
typedef bool      boolean;
typedef uint8_t   byte;
typedef uint16_t  word;

#ifdef EXTENDED_PIN_MODE
// Platforms who wnat to declare more than 256 pins need to define EXTENDED_PIN_MODE globally
typedef uint32_t pin_size_t;
#else
typedef uint8_t pin_size_t;
#endif

#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

long random(long);
long random(long, long);
void randomSeed(unsigned long);
long map(long, long, long, long, long);

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

#define digitalPinToPort(pin)       (0)
#define digitalPinToBitMask(pin)    (1UL << (pin))
#define digitalPinToTimer(pin)      (0)
#define digitalPinToInterrupt(pin)  (pin)
#define NOT_AN_INTERRUPT            (-1)
#define portOutputRegister(port)    ((volatile uint32_t*) sio_hw->gpio_out)
#define portInputRegister(port)     ((volatile uint32_t*) sio_hw->gpio_in)
#define portModeRegister(port)      ((volatile uint32_t*) sio_hw->gpio_oe)
#define digitalWriteFast(pin, val)  (val ? sio_hw->gpio_set = (1 << pin) : sio_hw->gpio_clr = (1 << pin))
#define digitalReadFast(pin)        ((1 << pin) & sio_hw->gpio_in)
#define sei() interrupts()
#define cli() noInterrupts()

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
