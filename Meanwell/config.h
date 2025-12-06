#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// CAN Configuration
const int CAN_CS_PIN = 10;

// LCD Configuration (4-bit mode)
const int LCD_RS = A2;
const int LCD_EN = A3;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = A4;

// Button and Switch Pins  
const int V_ENC_CLK = 6;       // Voltage rotary encoder CLK pin
const int V_ENC_DT = 7;        // Voltage rotary encoder DT pin
const int I_ENC_CLK = A0;      // Current rotary encoder CLK pin
const int I_ENC_DT = 9;        // Current rotary encoder DT pin
const int SET_BUF_PIN = A1;    // Pushbutton to set buffer
const int OP_TOGGLE_PIN = A5;  // Pushbutton to toggle output
const int VIEW_PIN = 8;
const int CHNG_I_PIN = 2;

// CAN Message IDs (from DBC file)
const unsigned long CAN_ID_TX = 0xC0300;  // ID for sending messages to power supply
const unsigned long CAN_ID_RX = 0x800C0200;  // ID for receiving messages from power supply

// Command values (from DBC file)
const uint16_t CMD_VOUT_SET = 0x20;   // 32
const uint16_t CMD_IOUT_SET = 0x30;   // 48
const uint16_t CMD_OPERATION = 0x00;  // 0
const uint16_t CMD_READ_VOUT = 0x60;  // 96
const uint16_t CMD_READ_IOUT = 0x61;  // 97
const uint16_t CMD_FAULT_STATUS = 0x40; // 64
const uint16_t CMD_READ_VIN = 0x50; // 80
const uint16_t CMD_READ_FAN_SPEED1 = 0x70; // 112
const uint16_t CMD_READ_FAN_SPEED2 = 0x71; // 113
const uint16_t CMD_SCALING_FACTOR = 0xC0; // 192
const uint16_t CMD_SYSTEM_STATUS = 0xC1; // 193
const uint16_t CMD_SYSTEM_CONFIG = 0xC2; // 194
const uint16_t CMD_DIRECTION_CTRL = 0x100; // 256
const uint16_t CMD_REVERSE_VOUT_SET = 0x120; // 288
const uint16_t CMD_REVERSE_IOUT_SET = 0x130; // 304
const uint16_t CMD_BIDIRECTIONAL_CONFIG = 0x140; // 320

// State Variables (extern declarations)
extern float vSet;
extern float iSetPos;
extern float iSetNeg;
extern float vBuf;
extern float iBufPos;
extern float iBufNeg;
extern bool editingIBufNeg;
extern float vOut;
extern float iOut;
extern bool outputEnabled;
extern bool toggle;
extern uint8_t displayMode;
extern float energyWh;
extern unsigned long lastEnergyUpdate;
extern unsigned long timerStartMillis;
extern bool lastOutputEnabled;
extern unsigned long pausedTime;
extern unsigned long canPacketsReceived;
extern float filteredPower;

#endif // CONFIG_H 