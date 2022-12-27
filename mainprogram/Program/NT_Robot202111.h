#ifndef NT_ROBOT202111_H
#define NT_ROBOT202111_H

//  NT_Robot202111.h
//  for Teemsy 3.5  Ver.1.2

#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.h"
// 以下定義ファイルが見つからないので適当に作成
#define A21 0
#define A22 0
#define A25 0
#define A26 0
#endif

constexpr int PIN_NATIVE_LED = 13;  // native LED

constexpr int Xbee_RX = 0;  // Serial1
constexpr int Xbee_TX = 1;
constexpr int OpenMV_RX = 7;  // Serial3
constexpr int OpenMV_TX = 8;
constexpr int IMU_RX = 9;  // Serial2
constexpr int IMU_TX = 10;

constexpr int PIN_LINE_LED = 30;
constexpr int LINE1D = 24;
constexpr int LINE1A = A21;
constexpr int LINE2D = 25;
constexpr int LINE2A = A22;
constexpr int LINE3D = 26;
constexpr int LINE3A = A25;
constexpr int LINE4D = 27;
constexpr int LINE4A = A26;
constexpr int LINE5D = 28;
constexpr int LINE5A = 39;

constexpr int StartSW = 16;
constexpr int GoalSW = 14;
constexpr int goal_setup = 14;
constexpr int SWG = 17;  // Switch LED Green
constexpr int SWR = 15;  // Switch LED Red

constexpr int Kicker = 11;  // Kicker
constexpr int Kick1 = 11;
constexpr int Kick_Dir = 12;

// constexpr int Ball = 5;  // Ball senser
// constexpr int Ball1 = 5;
// constexpr int Ball2 = 6;

constexpr int INT_29 = 29;

constexpr int Vbatt = 33;  // Battery Voltage
                           // 0.01811V/count

constexpr int PIN_LED_B = 20;
constexpr int PIN_LED_Y = 21;
constexpr int PIN_LED_G = 22;
constexpr int PIN_LED_R = 23;

constexpr int Aux1 = 40;
constexpr int Aux2 = 41;
// constexpr int Aux3 = 52;
// constexpr int Aux4 = 53;

// constexpr uint8_t I2C_PCF8574 = 0x20;
constexpr uint8_t TOF_1 = 0x52;
constexpr uint8_t TOF_2 = 0x54;
constexpr uint8_t TOF_3 = 0x56;
constexpr uint8_t TOF_4 = 0x58;
constexpr uint8_t TOF_5 = 0x5A;
constexpr uint8_t TOF_6 = 0x5C;

constexpr uint8_t I2C_LCD = 0x7C;

#endif
