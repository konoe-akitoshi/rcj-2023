#ifndef UTILS_PRINTER_HPP
#define UTILS_PRINTER_HPP

#ifdef LOCAL_INCLUDE
#include "../../local/arduino_deps.hpp"
#endif

#include "../types/vector2.hpp"

namespace util
{
inline void PrintInt(const char* name, const int i, bool next_line = true) {
    Serial.print(name);
    Serial.print(": ");
    if (next_line) {
        Serial.println(i);
    } else {
        Serial.print(i);
    }
}

inline void PrintFloat(const char* name, const int f, bool next_line = true) {
    Serial.print(name);
    Serial.print(": ");
    if (next_line) {
        Serial.println(f);
    } else {
        Serial.print(f);
    }
}

inline void PrintBool(const char* name, const bool b, bool next_line = true) {
    Serial.print(name);
    Serial.print(": ");
    if (next_line) {
        Serial.println(b ? "true" : "false");
    } else {
        Serial.print(b ? "true" : "false");
    }
}

inline void PrintVector(const char* name, const Vector2 vec, bool next_line = true) {
    Serial.print(name);
    Serial.print(": (");
    Serial.print(vec.x);
    Serial.print(", ");
    Serial.print(vec.y);
    if (next_line) {
        Serial.println(")");
    } else {
        Serial.print(")");
    }
}
}  // namespace util

#endif
