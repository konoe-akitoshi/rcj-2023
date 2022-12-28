#ifndef NT_ROBOT202111_H
#define NT_ROBOT202111_H

//  NT_Robot202111.h
//  for Teemsy 3.5  Ver.1.2

#ifdef LOCAL_INCLUDE
#include "../local/arduino_deps.h"
#endif

constexpr int Xbee_RX = 0;  // Serial1
constexpr int Xbee_TX = 1;
constexpr int OpenMV_RX = 7;  // Serial3
constexpr int OpenMV_TX = 8;
constexpr int IMU_RX = 9;  // Serial2
constexpr int IMU_TX = 10;

constexpr int goal_setup = 14;

constexpr int Kicker = 11;  // Kicker
constexpr int Kick1 = 11;
constexpr int Kick_Dir = 12;

constexpr int Aux1 = 40;
constexpr int Aux2 = 41;

constexpr uint8_t TOF_1 = 0x52;
constexpr uint8_t TOF_2 = 0x54;
constexpr uint8_t TOF_3 = 0x56;
constexpr uint8_t TOF_4 = 0x58;
constexpr uint8_t TOF_5 = 0x5A;
constexpr uint8_t TOF_6 = 0x5C;

constexpr uint8_t I2C_LCD = 0x7C;

#endif
