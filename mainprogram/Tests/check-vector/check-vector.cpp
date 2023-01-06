#ifdef PREVENT_SORCING_FROM_ARDUINO
#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.hpp"
#else
#endif

#include <vector>
#include "utils/serial_reader.hpp"

util::SerialReader Reader;
std::vector<int> vec;

void setup() {
    vec.resize(1);
    vec[0] = 10;
    Serial.begin(9600);
}

int cnt = 0;
int size = 3;

void loop() {
    if (vec.size() < 3) {
        vec.push_back(vec.back() + 1);
    } else if (cnt == 0) {
        Serial.println("input number...");
        int t = Reader.ReadInt();
        vec.push_back(t);
        size++;
    }
    delay(500);
    Serial.println(vec[cnt]);
    cnt++;
    cnt %= size;
}

#endif
