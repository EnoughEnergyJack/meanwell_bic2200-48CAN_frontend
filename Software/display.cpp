#include "display.h"
#include "config.h"
#include <LiquidCrystal.h>

extern LiquidCrystal lcd;

void updateDisplay1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V: ");
  lcd.print(vOut, 2);
  lcd.print("V");
  lcd.setCursor(11,0);
  lcd.print("sec:");
  // Timer in seconds (top right)
  unsigned long seconds = 0;
  if (outputEnabled) {
    seconds = (millis() - timerStartMillis) / 1000;
  } else {
    seconds = pausedTime / 1000;
  }
  char timerStr[7];
  snprintf(timerStr, sizeof(timerStr), "%lu", seconds);
  int timerLen = strlen(timerStr);
  lcd.setCursor(16 - timerLen, 1);
  lcd.print(timerStr);
  lcd.setCursor(0, 1);
  lcd.print("I: ");
  lcd.print(iOut, 2);
  lcd.print("A");
  lcd.setCursor(0, 2);
  lcd.print("P: ");
  lcd.print(filteredPower, 2);
  lcd.print("W");
  lcd.setCursor(0, 3);
  lcd.print("E: ");
  lcd.print(energyWh,0);
  lcd.print("Wh");
  lcd.setCursor(12, 2);
  lcd.print("OP:");
  lcd.setCursor(12, 3);
  if (outputEnabled) {
    lcd.print("ON ");
  } else {
    lcd.print("OFF");
  }
  //Serial.println(canPacketsReceived);
  // Blinking square at position (2, 15) - every 5 packets
  if ((canPacketsReceived/5) % 2 == 1) {
    lcd.setCursor(11, 3);
    lcd.print("[");
    lcd.setCursor(15, 3);
    lcd.print("]");
  }
}

void updateDisplay2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LIMITS");
  lcd.setCursor(12, 0);
  lcd.print("BUF");
  lcd.setCursor(0, 1);
  lcd.print("Vs : ");
  lcd.print(vSet, 1);
  lcd.setCursor(10, 1);
  lcd.print("<");
  lcd.setCursor(12, 1);
  lcd.print(vBuf, 1);
  lcd.setCursor(0, 2);
  lcd.print("I+ : ");
  lcd.print(iSetPos, 1);
  lcd.setCursor(10, 2);
  lcd.print(editingIBufNeg ? "  " : "<");
  lcd.setCursor(12, 2);
  lcd.print(iBufPos, 1);
  lcd.setCursor(0, 3);
  lcd.print("I- :");
  lcd.print(iSetNeg, 1);
  lcd.setCursor(10, 3);
  lcd.print(editingIBufNeg ? "<" : "  ");
  lcd.setCursor(11, 3);
  lcd.print(iBufNeg, 1);
}

void updateEnergyAndState() {
  // Output state transition logic
  if (outputEnabled && !lastOutputEnabled) {
    // Transition OFF -> ON: Reset timer and energy meter
    timerStartMillis = millis();
    energyWh = 0.0;
    lastEnergyUpdate = millis();
    pausedTime = 0;
  } else if (!outputEnabled && lastOutputEnabled) {
    // Transition ON -> OFF: Store the elapsed time
    pausedTime = millis() - timerStartMillis;
    timerStartMillis = 0;
  }
  lastOutputEnabled = outputEnabled;
  
  // Energy integration only if ON
  if (outputEnabled) {
    unsigned long now = millis();
    float dt = (now - lastEnergyUpdate) / 1000.0;
    if (dt > 0.1) {
      float power = vOut * iOut;
      energyWh += (power * dt) / 3600.0;
      lastEnergyUpdate = now;
    }
  }
} 
