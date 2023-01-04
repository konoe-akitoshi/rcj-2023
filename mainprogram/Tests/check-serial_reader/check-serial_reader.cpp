#ifdef PREVENT_SORCING_FROM_ARDUINO

#include "utils/serial_reader.hpp"

const util::SerialReader Reader;

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println("Input int...");
    int i = Reader.ReadInt();
    Serial.print("read: ");
    Serial.println(i);

    Serial.println("Input float...");
    float f = Reader.ReadFloat();
    Serial.print("read: ");
    Serial.println(f);
}

#endif
