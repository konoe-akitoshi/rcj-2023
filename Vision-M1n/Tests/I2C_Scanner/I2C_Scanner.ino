#include <Wire.h>

void setup() {
    do {
        Serial.begin(9600);
        delay(100);
    } while (!Serial);
    Serial.println("Open Serial(9600)");

    Wire.setSDA(8);
    Wire.setSCL(9);
    Wire.begin();
    Serial.println("Start I2C Scanner");
}

void print_address(const byte addr, const char* msg) {
    Serial.print("address: 0x");
    if (addr < 16) Serial.print("0");
    Serial.print(addr, HEX);
    Serial.print(" | ");
    Serial.println(msg);
}

void loop() {
    Serial.println();
    Serial.println("Scanning...");

    int device_count = 0;
    for (byte address = 1; address < 127; ++address) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        const int status = Wire.endTransmission();

        switch (status) {
            case 0:
                print_address(address, "success.");
                device_count += 1;
                break;
            case 1:
                print_address(address, "data too long to fit in transmit buffer.");
                break;
            case 2:
                print_address(address, "received NACK on transmit of address.");
                break;
            case 3:
                print_address(address, "received NACK on transmit of data.");
                break;
            case 4:
                print_address(address, "other error.");
                break;
            case 5:
                print_address(address, "timeout.");
                break;
            default:
                print_address(address, "Unknown status");
                break;
        }
    }
    if (device_count == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.print("Found ");
        Serial.print(device_count);
        Serial.println(" devices");
    }

    delay(5000);
}
