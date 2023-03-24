#include "module/xbee.hpp"

class SerialReader
{
  public:
    explicit constexpr SerialReader() {}
    int readInt() const {
        char buf[40] = {0};
        wait_();
        int ret = 0;
        int size = min(40, Serial.available());
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

  private:
    void wait_() const {
        while (Serial.available() == 0) {
            ;
        }
        delay(50);
    }
};

constexpr SerialReader Reader;

void setup() {
    do {
        Serial.begin(9600);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    XBee.setup();
}

void loop() {
    Serial.print("Input number: ");
    const int number = Reader.readInt();
    Serial.println(number);
    XBee.sendData(number);

    while (XBee.hasData()) {
        Serial.println(XBee.readData());
    }
}
