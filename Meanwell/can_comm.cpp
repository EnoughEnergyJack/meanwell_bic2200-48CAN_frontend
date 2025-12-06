#include "can_comm.h"
#include "config.h"
#include <mcp_can.h>
#include <SPI.h>

extern MCP_CAN CAN;

unsigned long lastEchoCheck = 0;
const unsigned long ECHO_CHECK_INTERVAL = 100; // 2 seconds
uint8_t echoState = 0; // 0: vSet, 1: iSetPos, 2: iSetNeg
float lastEchoValue = 0.0;
uint16_t lastEchoCmd = 0;

void checkSetpointEcho() {
  if (millis() - lastEchoCheck < ECHO_CHECK_INTERVAL) return;
  lastEchoCheck = millis();
  uint8_t data[2];
  switch (echoState) {
    case 0: // Voltage
      data[0] = CMD_VOUT_SET & 0xFF;
      data[1] = (CMD_VOUT_SET >> 8) & 0xFF;
      lastEchoCmd = CMD_VOUT_SET;
      break;
    case 1: // Positive current
      data[0] = CMD_IOUT_SET & 0xFF;
      data[1] = (CMD_IOUT_SET >> 8) & 0xFF;
      lastEchoCmd = CMD_IOUT_SET;
      break;
    case 2: // Negative current
      data[0] = CMD_REVERSE_IOUT_SET & 0xFF;
      data[1] = (CMD_REVERSE_IOUT_SET >> 8) & 0xFF;
      lastEchoCmd = CMD_REVERSE_IOUT_SET;
      break;
  }
  CAN.sendMsgBuf(CAN_ID_TX, 1, 2, data);
  echoState = (echoState + 1) % 3;
// Serial.print(data[0],HEX);
// Serial.println(data[1],HEX);
  //delay(50);
  // Wait for a response for up to 50ms
  unsigned long start = millis();
  while (millis() - start < 50) {
    if (CAN.checkReceive() == CAN_MSGAVAIL) {
      unsigned long rxId;
      unsigned char len;
      unsigned char rxBuf[8];
      CAN.readMsgBuf(&rxId, &len, rxBuf);
      canPacketsReceived++; // Increment packet counter
      // Serial.print(rxBuf[0],HEX);
      // Serial.print(rxBuf[1],HEX);
      // Serial.print(rxBuf[2],HEX);
      // Serial.println(rxBuf[3],HEX);
      if (rxId == CAN_ID_RX && len == 4) {
        uint16_t command = (rxBuf[0]) | (rxBuf[1] << 8);
        float value = (rxBuf[3] << 8 | rxBuf[2]) * 0.01;
          if (command == CMD_VOUT_SET) {
            vSet = value;
          } else if (command == CMD_IOUT_SET) {
            iSetPos = value;
          } else if (command == CMD_REVERSE_IOUT_SET) {
            iSetNeg = -value;
          }
          break; // Stop waiting after a valid response
      }
    }
  }
}

void checkOutputReal() {
  static unsigned long lastRequestTime = 0;
  const unsigned long REQUEST_INTERVAL = 100;

  if (millis() - lastRequestTime >= REQUEST_INTERVAL) {
    byte data[2] = { 0 };
    if(toggle==1){
      data[0] = CMD_READ_VOUT;
      data[1] = (CMD_READ_VOUT >> 8);
      CAN.sendMsgBuf(CAN_ID_TX, 1, 2, data);
    }
    if(toggle==0){
      data[0] = CMD_READ_IOUT;
      data[1] = (CMD_READ_IOUT >> 8);
      CAN.sendMsgBuf(CAN_ID_TX, 1, 2, data);
    }
    toggle=!toggle;
    lastRequestTime = millis();
    //delay(10);
  }

  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    unsigned long rxId;
    unsigned char len;
    unsigned char rxBuf[8];
    CAN.readMsgBuf(&rxId, &len, rxBuf);
    canPacketsReceived++; // Increment packet counter
    if (rxId == CAN_ID_RX && len == 4) {
      uint16_t command = (rxBuf[0]);
      switch (command) {
        case CMD_READ_VOUT:
          vOut = (rxBuf[3] << 8 | rxBuf[2]) * 0.01;
          // Calculate power when voltage is updated
          float newPowerV = vOut * iOut;
          filteredPower = 0.98f * filteredPower + 0.02f * newPowerV;
          break;
        case CMD_READ_IOUT:
          iOut = (rxBuf[3] << 8 | rxBuf[2]) * 0.01;
          // Calculate power when current is updated
          float newPowerI = vOut * iOut;
          filteredPower = 0.98f * filteredPower + 0.02f * newPowerI;
          break;
      }
    }
    while (CAN.checkReceive() == CAN_MSGAVAIL) {
      CAN.readMsgBuf(&rxId, &len, rxBuf);
    }
  }
}

void sendSetpoints() {
  uint8_t data[4] = { 0 };
  delay(5);
  data[0] = CMD_VOUT_SET;
  data[1] = 0;
  data[3] = (uint8_t)((uint16_t)(vSet * 100) >> 8);
  data[2] = (uint8_t)(vSet * 100);
  CAN.sendMsgBuf(CAN_ID_TX, 1, 4, data);
  delay(50);
  data[0] = CMD_IOUT_SET;
  data[1] = 0;
  data[3] = (uint8_t)((uint16_t)(iSetPos*100) >> 8);
  data[2] = (uint8_t)(iSetPos*100);
  // Serial.print(data[0], HEX);
  // Serial.print(data[1], HEX);
  // Serial.print(data[2], HEX);
  // Serial.println(data[3], HEX);
  CAN.sendMsgBuf(CAN_ID_TX, 1, 4, data);
  // Send negative current setpoint
  delay(50);
  data[0] = CMD_REVERSE_IOUT_SET;
  data[1] = 1;
  data[3] = (uint8_t)((uint16_t)(abs(iSetNeg) * 100) >> 8);
  data[2] = (uint8_t)(abs(iSetNeg) * 100);
  CAN.sendMsgBuf(CAN_ID_TX, 1, 4, data);
  //delay(50);
}

void sendOutputState() {
  uint8_t data[3] = { 0 };
  delay(10);
  data[0] = CMD_OPERATION & 0xFF;
  data[1] = (CMD_OPERATION >> 8) & 0xFF;
  data[2] = outputEnabled ? 1 : 0;
  CAN.sendMsgBuf(CAN_ID_TX, 1, 3, data);
} 