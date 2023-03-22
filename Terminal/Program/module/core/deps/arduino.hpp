#ifndef MODULE_CORE_DEPS_ARDUINO_HPP
#define MODULE_CORE_DEPS_ARDUINO_HPP

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CORE_HAS_LIBB64

typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;

#define LOW (0x0)
#define HIGH (0x1)
#define INPUT (0x0)
#define OUTPUT (0x1)
#define INPUT_PULLUP (0x2)
#define INPUT_PULLDOWN (0x3)
#define CHANGE 2
#define FALLING 3
#define RISING 4
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352
#define SERIAL 0x0
#define DISPLAY 0x1
enum BitOrder
{
    LSBFIRST = 0,
    MSBFIRST = 1
};

#define clockCyclesPerMicrosecond() (SystemCoreClock / 1000000L)
#define clockCyclesToMicroseconds(a) (((a)*1000L) / (SystemCoreClock / 1000L))
#define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))

void yield(void);

/* system functions */
int main(void);
void init(void);

/* sketch */
void setup(void);
void loop(void);

void tone(uint32_t _pin, uint32_t frequency, uint32_t duration = 0);
void noTone(uint32_t _pin);

extern long random(long);
extern long random(long, long);
extern void randomSeed(uint32_t dwSeed);
extern long map(long, long, long, long, long);
extern uint16_t makeWord(uint16_t w);
extern uint16_t makeWord(uint8_t h, uint8_t l);

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

extern unsigned long millis(void);
extern unsigned long micros(void);
extern void delay(unsigned long dwMs);

#define D0 (0ul)
#define D1 (1ul)
#define D2 (2ul)
#define D3 (3ul)
#define D4 (4ul)
#define D5 (5ul)
#define D6 (6ul)
#define D7 (7ul)
#define D8 (8ul)

static const uint8_t A0 = D0;
static const uint8_t A1 = D1;
static const uint8_t A2 = D2;
static const uint8_t A3 = D3;
static const uint8_t A4 = D4;
static const uint8_t A5 = D5;
static const uint8_t A6 = D6;
static const uint8_t A7 = D7;
static const uint8_t A8 = D8;

#define BUTTON_1 (28ul)
#define BUTTON_2 (29ul)
#define BUTTON_3 (30ul)
#define WIO_KEY_A (28ul)
#define WIO_KEY_B (29ul)
#define WIO_KEY_C (30ul)

#define SWITCH_X (31ul)
#define SWITCH_Y (32ul)
#define SWITCH_Z (33ul)
#define SWITCH_B (34ul)
#define SWITCH_U (35ul)
#define WIO_5S_UP (31ul)
#define WIO_5S_LEFT (32ul)
#define WIO_5S_RIGHT (33ul)
#define WIO_5S_DOWN (34ul)
#define WIO_5S_PRESS (35ul)

extern void pinMode(uint32_t dwPin, uint32_t dwMode);
extern void digitalWrite(uint32_t dwPin, uint32_t dwVal);
extern int digitalRead(uint32_t ulPin);
extern void analogWrite(uint32_t ulPin, uint32_t ulValue);
extern uint32_t analogRead(uint32_t ulPin);
extern void analogReadResolution(int res);
extern void analogWriteResolution(int res);
extern void analogOutputInit(void);

extern uint32_t shiftIn(uint32_t ulDataPin, uint32_t ulClockPin, uint32_t ulBitOrder);
extern void shiftOut(uint32_t ulDataPin, uint32_t ulClockPin, uint32_t ulBitOrder, uint32_t ulVal);

void pwm(uint32_t _pin, uint32_t frequency, uint32_t duty = 500);
void noPwm(uint32_t _pin);

typedef void (*voidFuncPtr)(void);
void attachInterrupt(uint32_t pin, voidFuncPtr callback, uint32_t mode);
void detachInterrupt(uint32_t pin);

template <class T, class L>
auto min(const T &a, const L &b) -> decltype((b < a) ? b : a) {
    return (b < a) ? b : a;
}

template <class T, class L>
auto max(const T &a, const L &b) -> decltype((b < a) ? b : a) {
    return (a < b) ? b : a;
}

#ifdef abs
#undef abs
#endif
#define abs(x) ((x) > 0 ? (x) : -(x))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x) * (x))

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#define lowByte(w) ((uint8_t)((w)&0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define bit(b) (1UL << (b))

class __FlashStringHelper;
class String;
class StringSumHelper;

class Serial_
{
  public:
    void begin(uint32_t baud_count);
    void begin(unsigned long, uint8_t);
    void beginWithoutDTR(uint32_t baud_count);
    void beginWithoutDTR(unsigned long, uint8_t);
    void end(void);
    virtual int available(void);
    virtual int availableForWrite(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual void clear(void);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(void);
    operator bool();
    size_t readBytes(char *buffer, size_t length);
    int32_t readBreak();
    uint32_t baud();
    uint8_t stopbits();
    uint8_t paritytype();
    uint8_t numbits();
    bool dtr();
    bool rts();
    enum
    {
        ONE_STOP_BIT = 0,
        ONE_AND_HALF_STOP_BIT = 1,
        TWO_STOP_BITS = 2,
    };
    enum
    {
        NO_PARITY = 0,
        ODD_PARITY = 1,
        EVEN_PARITY = 2,
        MARK_PARITY = 3,
        SPACE_PARITY = 4,
    };
};
extern Serial_ Serial;

class Uart
{
  public:
    void begin(unsigned long baudRate);
    void begin(unsigned long baudrate, uint16_t config);
    void end();
    int available();
    int availableForWrite();
    int peek();
    int read();
    void flush();
    size_t write(const uint8_t data);
    void IrqHandler();
    operator bool() { return true; }
};

extern Uart Serial1;
extern Uart Serial2;

#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))


class String
{
    typedef void (String::*StringIfHelperType)() const;
    void StringIfHelper() const {}

  public:
    String(const char *cstr = "");
    String(const String &str);
    String(const __FlashStringHelper *str);
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    String(String &&rval);
    String(StringSumHelper &&rval);
#endif
    explicit String(char c);
    explicit String(unsigned char, unsigned char base = 10);
    explicit String(int, unsigned char base = 10);
    explicit String(unsigned int, unsigned char base = 10);
    explicit String(long, unsigned char base = 10);
    explicit String(unsigned long, unsigned char base = 10);
    explicit String(float, unsigned char decimalPlaces = 2);
    explicit String(double, unsigned char decimalPlaces = 2);
    ~String(void);
    static String format(const char *format, ...);
    unsigned char reserve(unsigned int size);
    inline unsigned int length(void) const { return len; }
    String &operator=(const String &rhs);
    String &operator=(const char *cstr);
    String &operator=(const __FlashStringHelper *str);
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    String &operator=(String &&rval);
    String &operator=(StringSumHelper &&rval);
#endif
    unsigned char concat(const String &str);
    unsigned char concat(const char *cstr);
    unsigned char concat(char c);
    unsigned char concat(unsigned char c);
    unsigned char concat(int num);
    unsigned char concat(unsigned int num);
    unsigned char concat(long num);
    unsigned char concat(unsigned long num);
    unsigned char concat(float num);
    unsigned char concat(double num);
    unsigned char concat(const __FlashStringHelper *str);
    String &operator+=(const String &rhs) {
        concat(rhs);
        return (*this);
    }
    String &operator+=(const char *cstr) {
        concat(cstr);
        return (*this);
    }
    String &operator+=(char c) {
        concat(c);
        return (*this);
    }
    String &operator+=(unsigned char num) {
        concat(num);
        return (*this);
    }
    String &operator+=(int num) {
        concat(num);
        return (*this);
    }
    String &operator+=(unsigned int num) {
        concat(num);
        return (*this);
    }
    String &operator+=(long num) {
        concat(num);
        return (*this);
    }
    String &operator+=(unsigned long num) {
        concat(num);
        return (*this);
    }
    String &operator+=(float num) {
        concat(num);
        return (*this);
    }
    String &operator+=(double num) {
        concat(num);
        return (*this);
    }
    String &operator+=(const __FlashStringHelper *str) {
        concat(str);
        return (*this);
    }
    friend StringSumHelper &operator+(const StringSumHelper &lhs, const String &rhs);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, const char *cstr);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, char c);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned char num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, int num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned int num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, long num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, unsigned long num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, float num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, double num);
    friend StringSumHelper &operator+(const StringSumHelper &lhs, const __FlashStringHelper *rhs);
    operator StringIfHelperType() const { return buffer ? &String::StringIfHelper : 0; }
    int compareTo(const String &s) const;
    unsigned char equals(const String &s) const;
    unsigned char equals(const char *cstr) const;
    unsigned char operator==(const String &rhs) const { return equals(rhs); }
    unsigned char operator==(const char *cstr) const { return equals(cstr); }
    unsigned char operator!=(const String &rhs) const { return !equals(rhs); }
    unsigned char operator!=(const char *cstr) const { return !equals(cstr); }
    unsigned char operator<(const String &rhs) const;
    unsigned char operator>(const String &rhs) const;
    unsigned char operator<=(const String &rhs) const;
    unsigned char operator>=(const String &rhs) const;
    unsigned char equalsIgnoreCase(const String &s) const;
    unsigned char equalsConstantTime(const String &s) const;
    unsigned char startsWith(const String &prefix) const;
    unsigned char startsWith(const String &prefix, unsigned int offset) const;
    unsigned char endsWith(const String &suffix) const;
    char charAt(unsigned int index) const;
    void setCharAt(unsigned int index, char c);
    char operator[](unsigned int index) const;
    char &operator[](unsigned int index);
    void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
    void toCharArray(char *buf, unsigned int bufsize, unsigned int index = 0) const { getBytes((unsigned char *)buf, bufsize, index); }
    const char *c_str() const { return buffer; }
    char *begin() { return buffer; }
    char *end() { return buffer + length(); }
    const char *begin() const { return c_str(); }
    const char *end() const { return c_str() + length(); }
    int indexOf(char ch) const;
    int indexOf(char ch, unsigned int fromIndex) const;
    int indexOf(const String &str) const;
    int indexOf(const String &str, unsigned int fromIndex) const;
    int lastIndexOf(char ch) const;
    int lastIndexOf(char ch, unsigned int fromIndex) const;
    int lastIndexOf(const String &str) const;
    int lastIndexOf(const String &str, unsigned int fromIndex) const;
    String substring(unsigned int beginIndex) const { return substring(beginIndex, len); };
    String substring(unsigned int beginIndex, unsigned int endIndex) const;
    void replace(char find, char replace);
    void replace(const String &find, const String &replace);
    void remove(unsigned int index);
    void remove(unsigned int index, unsigned int count);
    void toLowerCase(void);
    void toUpperCase(void);
    void trim(void);
    long toInt(void) const;
    float toFloat(void) const;
    double toDouble(void) const;

  protected:
    char *buffer;           // the actual char array
    unsigned int capacity;  // the array length minus one (for the '\0')
    unsigned int len;       // the String length (not counting the '\0')
  protected:
    void init(void);
    void invalidate(void);
    unsigned char changeBuffer(unsigned int maxStrLen);
    unsigned char concat(const char *cstr, unsigned int length);
    String &copy(const char *cstr, unsigned int length);
    String &copy(const __FlashStringHelper *pstr, unsigned int length);
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
    void move(String &rhs);
#endif
};

class StringSumHelper : public String
{
  public:
    StringSumHelper(const String &s) : String(s) {}
    StringSumHelper(const char *p) : String(p) {}
    StringSumHelper(char c) : String(c) {}
    StringSumHelper(unsigned char num) : String(num) {}
    StringSumHelper(int num) : String(num) {}
    StringSumHelper(unsigned int num) : String(num) {}
    StringSumHelper(long num) : String(num) {}
    StringSumHelper(unsigned long num) : String(num) {}
    StringSumHelper(float num) : String(num) {}
    StringSumHelper(double num) : String(num) {}
};

#endif
