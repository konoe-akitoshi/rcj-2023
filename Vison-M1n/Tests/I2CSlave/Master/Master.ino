#include <utility>
#include <Wire.h>

constexpr uint8_t I2C_ADDRESS = 0x10;
constexpr uint8_t I2C_SDA = 4;
constexpr uint8_t I2C_SCL = 5;

class SerialReader
{
  public:
    explicit constexpr SerialReader() {}
    int readInt() const {
        static char buf[40] = {0};
        wait_();
        int ret = 0;
        int size = Serial.available();
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


/*
|     ID | operation                   |
| -----: | :-------------------------- |
|      1 | Return {1}                  |
|      2 | Return {1, 2}               |
|      3 | Return {1, 2, 3}            |
|      4 | Return {1, 2, 3, 4}         |
|     2n | Return recieved value       |
| 2n + 1 | Return recieved value twice |
*/

// First: id, Second: count
std::pair<int, int> sendId(const uint8_t address) {
    Serial.print("Input number for 0x");
    Serial.print(address, 16);
    Serial.print(": ");
    uint8_t id = Reader.readInt();
    Serial.println(id);

    Wire.beginTransmission(address);
    Wire.write(id);
    Wire.endTransmission();

    if (id == 1) {
        return {1, 1};
    } else if (id == 2) {
        return {2, 2};
    } else if (id == 3) {
        return {3, 3};
    } else if (id == 4) {
        return {4, 4};
    } else if (id % 2 == 0) {
        return {id, 1};
    } else { // id % 2 == 1
        return {id, 2};
    }
}

enum class Status {
    Ok,
    Error,
    Timeout
};

// 返り値 Status にするとコンパイルできない (なんで?)
auto readData(const uint8_t address, const int id, const int count) {
    static uint8_t data[4] = {0};
    Wire.requestFrom(address, count);
    bool timeout = false;
    for (int time = 0; !Wire.available(); ++time) {
        if (time < 10) {
            delay(1);
        } else {
            timeout = true;
            break;
        }
    }
    if (timeout) {
        return Status::Timeout;
    }
    for (int cnt = 0; cnt < count; ++cnt) {
        data[cnt] = Wire.read();
    }

    Serial.print("received: ");
    for (int i = 0; i < count; ++i) {
        Serial.print(data[i]);
        Serial.print(",");
    }
    Serial.println();

    if (id == 1 && count == 1) {
        return data[0] == 1 ? Status::Ok : Status::Error;
    } else if (id == 2 && count == 2) {
        bool t1 = data[0] == 1;
        bool t2 = data[1] == 2;
        return (t1 && t2) ? Status::Ok : Status::Error;
    } else if (id == 3 && count == 3) {
        bool t1 = data[0] == 1;
        bool t2 = data[1] == 2;
        bool t3 = data[2] == 3;
        return (t1 && t2 && t3) ? Status::Ok : Status::Error;
    } else if (id == 4 && count == 4) {
        bool t1 = data[0] == 1;
        bool t2 = data[1] == 2;
        bool t3 = data[2] == 3;
        bool t4 = data[3] == 4;
        return (t1 && t2 && t3 && t4) ? Status::Ok : Status::Error;
    } else if (id % 2 == 0 && count == 1) {
        return data[0] == id ? Status::Ok : Status::Error;
    } else if (id % 2 == 1 && count == 2) {
        bool t1 = data[0] == id;
        bool t2 = data[1] == id;
        return (t1 && t2) ? Status::Ok : Status::Error;
    }
    return Status::Error;
}

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    Wire.setSDA(I2C_SDA);
    Serial.println("1");
    Wire.setSCL(I2C_SCL);
    Serial.println("2");
    Wire.setClock(400000);
    Serial.println("3");
    Wire.begin();
    Serial.println("Wire bigin");
}

void loop() {
    auto s = sendId(I2C_ADDRESS);
    auto status = readData(I2C_ADDRESS, s.first, s.second);
    if (status == Status::Timeout) {
        Serial.println("TIMEOUT");
    } else if (status == Status::Error) {
        Serial.println("ERROR");
    } else {
        Serial.println("OK");
    }
    delay(500);
}
