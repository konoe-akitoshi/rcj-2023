//
//  NT_Robot202011.h
//  for Teemsy 3.5  Ver.1.1
//  11.Apr.2020 By H.Saeki
//  26.Oct.2020 By H.Saeki
//  13.Nov.2020 By H.Saeki
//

const int ledPin = 13; // native LED

const int Xbee_RX = 0; // Serial1
const int Xbee_TX = 1;
const int OpenMV_RX = 7; // Serial3
const int OpenMV_TX = 8;
const int IMU_RX = 9; // Serial2
const int IMU_TX = 10;

const int LINE_LED = 30;
const int LINE1D = 24;
const int LINE1A = A21;
const int LINE2D = 25;
const int LINE2A = A22;
const int LINE3D = 26;
const int LINE3A = A25;
const int LINE4D = 27;
const int LINE4A = A26;
const int LINE5D = 28;
const int LINE5A = 39;

const int StartSW = 16;
const int GoalSW = 14;
const int goal_setup = 14;
const int SWG = 17;  // Switch LED Green
const int SWR = 15;  // Switch LED Red

const int Kicker = 11; // Kicker
const int Kick1  = 11;
const int Kick_Dir = 12;

const int Ball = 5;  // Ball senser
const int Ball1 = 5;
const int Ball2 = 6;

const int INT_29 = 29;

const int Vbatt = 33; // Battery Voltage
			    // 0.01811V/count

const int LED_B = 20;
const int LED_Y = 21;
const int LED_G = 22;
const int LED_R = 23;

const int Aux1 = 40;
const int Aux2 = 41;
const int Aux3 = 52;
const int Aux4 = 53;

const uint8_t I2C_PCF8574 = 0x20;
const uint8_t TOF_1 = 0x52;
const uint8_t TOF_2 = 0x54;
const uint8_t TOF_3 = 0x56;
const uint8_t TOF_4 = 0x58;
const uint8_t TOF_5 = 0x5A;
const uint8_t TOF_6 = 0x5C;


const uint8_t I2C_LCD = 0x7C;
