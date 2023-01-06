#ifndef TESTS_UTIL_READER_HPP
#define TESTS_UTIL_READER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

namespace util
{
class SerialReader
{
  public:
    explicit SerialReader() {}
    int ReadInt() const {
        Wait_();
        int ret = 0;
        int size = Serial.available();
        char buf[size];
        for (int i = 0; i < size; ++i) {
            buf[i] = (char)Serial.read();
        }
        int now = 0;
        char c = buf[now];
        while ((c < '0' || '9' < c) && c != '-') {
            c = buf[++now];
        }
        const bool f = (c == '-') && (c = buf[++now]);
        while (now + 1 < size) {
            ret = 10 * ret + c - '0';
            c = buf[++now];
        }
        return f ? -ret : ret;
    }

    float ReadFloat() const {
        Wait_();
        int size = Serial.available();
        char buf[size];
        for (int i = 0; i < size; ++i) {
            buf[i] = (char)Serial.read();
        }
        int dot_idx = size - 1;
        for (int i = 0; i < size; ++i) {
            if (buf[i] == '.') {
                dot_idx = i;
                break;
            }
        }
        float ret = 0;
        float e = pow(10, dot_idx - 1);
        int now = 0;
        char c = buf[now];
        const bool f = (buf[now] == '-') && (c = buf[++now], e /= 10);
        while (now + 1 < size) {
            if (c != '.') {
                ret += (c - '0') * e;
                e /= 10;
            }
            c = buf[++now];
        }
        return f ? -ret : ret;
    }

  private:
    void Wait_() const {
        while (Serial.available() == 0) {
            ;
        }
        delay(50);
    }
};
}  // namespace util

#endif
