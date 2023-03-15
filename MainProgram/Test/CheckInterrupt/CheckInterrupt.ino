/*
 * 自作モジュール群に依存しないバージョン、ピン番号等は要確認。
 * 13行目 test_pin (INPUT_PULLUP) が HIGH の時 # LOWの時 . がシリアルモニタに出力される
 */

/*
constexpr int LineSensorLed = 22;

constexpr int Interrupt = 28;
constexpr int LineSensor1 = 16;
constexpr int LineSensor2 = 17;
constexpr int LineSensor3 = 18;
constexpr int LineSensor4 = 19;
constexpr int LineSensor5 = 20;

constexpr int test_pin = Interrupt;

void setup() {
    Serial.begin(9600);
    pinMode(LineSensorLed, OUTPUT);
    pinMode(test_pin, INPUT_PULLUP);

    digitalWrite(LineSensorLed, HIGH);
}

void loop() {
    static long count = 0;
    digitalWrite(LineSensorLed, HIGH);
    count += 1;
    const bool state = (digitalRead(test_pin) == HIGH);
    if (count % 200) {
        Serial.print(state ? "#" : ".");
    } else {
        Serial.println(state ? "#" : ".");
    }
}
//*/
