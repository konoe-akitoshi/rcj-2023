#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#else
#endif

#include <utility>
#include "utils/serial_reader.hpp"

std::pair<int, int> p;
const util::SerialReader Reader;
bool toggle = false;

void setup() {
    p = {0, 0};
    Serial.begin(9600);
}

void loop() {
    int read = Reader.ReadInt();
    if (toggle) {
        p.first = read;
    } else {
        p.second = read;
    }
    toggle ^= true;

    Serial.print("data: ");
    Serial.print(p.first);
    Serial.print(", ");
    Serial.println(p.second);
}

#endif
