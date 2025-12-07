#include "inputs.h"
#include "config.h"
#include "can_comm.h"
#include "display.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Encoder.h>
extern LiquidCrystal lcd;

// Create encoder objects
Encoder voltageEncoder(V_ENC_CLK, V_ENC_DT);
Encoder currentEncoder(I_ENC_CLK, I_ENC_DT);

// Encoder position tracking
static long lastVoltagePosition = 0;
static long lastCurrentPosition = 0;

void readInputs() {
  static unsigned long lastModeSwitch = 0;
  const unsigned long modeSwitchDebounce = 500;
  if (digitalRead(VIEW_PIN) == LOW) {
    if (millis() - lastModeSwitch > modeSwitchDebounce) {
      displayMode = !displayMode;
      lastModeSwitch = millis();
    }
    return;
  }
  static unsigned long lastIBufSwitch = 0;
  const unsigned long iBufSwitchDebounce = 500;
  if (digitalRead(CHNG_I_PIN) == LOW) {
    if (millis() - lastIBufSwitch > iBufSwitchDebounce) {
      editingIBufNeg = !editingIBufNeg;
      lastIBufSwitch = millis();
    }
    return;
  }
  // Read voltage encoder - simplified like the example
  long newVoltagePosition = int(voltageEncoder.read()/4);
  if (newVoltagePosition != lastVoltagePosition) {
    
    if (newVoltagePosition < lastVoltagePosition) {
      vBuf += 0.1;
      if (vBuf > 65) {
        vBuf = 65;
      }
    } else {
      vBuf -= 0.1;
      if (vBuf < 38) {
        vBuf = 38;
      }
    }
    
    lastVoltagePosition = newVoltagePosition;
    return;
  }
  // Read current encoder - simplified like the example
  long newCurrentPosition = int(currentEncoder.read()/4);
  if (newCurrentPosition != lastCurrentPosition) {
    
    if (newCurrentPosition < lastCurrentPosition) {
      // Clockwise - increase current
      if (editingIBufNeg) {
        iBufNeg -= 0.1;
        if (iBufNeg < -38) iBufNeg = -38;
      } else {
        iBufPos += 0.1;
        if (iBufPos > 45) iBufPos = 45;
      }
    } else {
      // Counter-clockwise - decrease current
      if (editingIBufNeg) {
        iBufNeg += 0.1;
        if (iBufNeg > -0) iBufNeg = -0.001;
      } else {
        iBufPos -= 0.1;
        if (iBufPos < 0) iBufPos = 0;
      }
    }
    
    lastCurrentPosition = newCurrentPosition;
    return;
  }
  if (digitalRead(SET_BUF_PIN) == LOW && displayMode) {
    vSet = vBuf;
    iSetPos = iBufPos;
    iSetNeg = iBufNeg;
    sendSetpoints();
    int i=0;
    while(i<6) {
    checkSetpointEcho();
    updateDisplay2();
    delay(50);
    i++;
    }
    delay(1500);
    if (outputEnabled) {
      displayMode=0;
    } 
  }
  if (digitalRead(OP_TOGGLE_PIN) == LOW) {
    outputEnabled = !outputEnabled;
    sendOutputState();
    // LCD feedback for output state
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("OUTPUT STATE :");
    lcd.setCursor(10, 2);
    if (outputEnabled) {
      lcd.print("ON");
      displayMode=0;
    } else {
      lcd.print("OFF");
    }
    delay(500);
  }
} 
