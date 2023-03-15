/*
void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while(!Serial);
    Serial.println("Open Serial(9600)");

    do {
        Serial2.begin(115200);
        delay(100);
    } while(!Serial2);
    Serial.println("Open Serial2(11520)");
}

void loop() {
    if (Serial2.available()) {
        int16_t data = 1000;
        while (Serial2.available()) {
            int8_t raw = (int8_t)Serial2.read();
            data = (int16_t)raw;
        }
        if (data != 1000) {
            Serial.print("Read: ");
            Serial.println(data);
        } else {
            Serial.println("Faild to read data");
        }
    } else {
        Serial.println("Serial2 not available");
    }
    delay(100);
}
//*/
