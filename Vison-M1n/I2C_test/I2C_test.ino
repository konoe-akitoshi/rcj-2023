#include <Wire.h>

void setup() {
  Wire.setSDA(8);  //()の中身はGP番号。配線によって異なります。
  Wire.setSCL(9);  //()の中身はGP番号。配線によって異なります。
  Wire.setClock(400000);
  Wire.begin();        // Wire communication begin
  Serial.begin(9600);  // The baudrate of Serial monitor is set in 9600
  while (!Serial)
    ;  // Waiting for Serial Monitor
  Serial.println("Starting...");
}

void loop() {
  uint8_t addr1 = 0x24;
  uint8_t addr2 = 0x25;
  uint8_t data1;
  uint8_t data2;

  Wire.requestFrom(addr1, 1);
  while (Wire.available()) {  // 要求より短いデータが来る可能性あり
    data1 = Wire.read();
    Serial.print("data1 = ");
    Serial.print(data1);
    Serial.println();
  }

  Wire.requestFrom(addr2, 1);
  while (Wire.available()) {  // 要求より短いデータが来る可能性あり
    data2 = Wire.read();
    Serial.print("data2 = ");
    Serial.print(data2);
    Serial.println();
  }

  delay(500);
}
