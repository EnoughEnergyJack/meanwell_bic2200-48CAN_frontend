#include "config.h"
#include "can_comm.h"
#include "display.h"
#include "inputs.h"
#include <mcp_can.h>
#include <SPI.h>
#include <LiquidCrystal.h>

// Global variable definitions
float vSet = 0.0;
float iSetPos = 0.0;
float iSetNeg = -0.0001;
float vBuf = 38.0;
float iBufPos = 0;
float iBufNeg = -0.001;
bool editingIBufNeg = false;
float vOut = 0.0;
float iOut = 0.0;
bool outputEnabled = false;
bool toggle = false;
uint8_t displayMode = 0;
float energyWh = 0.0;
unsigned long lastEnergyUpdate = 0;
unsigned long timerStartMillis = 0;
bool lastOutputEnabled = false;
unsigned long pausedTime = 0;
unsigned long canPacketsReceived = 0;
float filteredPower = 0.0;

MCP_CAN CAN(CAN_CS_PIN);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 4);
  lcd.clear();
  lcd.print("Meanwell Ctrl");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  if(CAN.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  CAN.setMode(MCP_NORMAL);
  sendOutputState();
  // pinMode(V_ENC_CLK, INPUT_PULLUP);
  // pinMode(V_ENC_DT, INPUT_PULLUP);
  // pinMode(I_ENC_CLK, INPUT_PULLUP);
  // pinMode(I_ENC_DT, INPUT_PULLUP);
  pinMode(SET_BUF_PIN, INPUT_PULLUP);
  pinMode(OP_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(VIEW_PIN, INPUT_PULLUP);
  pinMode(CHNG_I_PIN, INPUT_PULLUP);
  updateDisplay1();
}

void loop() {
  // Read inputs first and frequently for responsiveness
  readInputs();
  
  // Only update slower tasks every few loops
  static unsigned long lastSlowUpdate = 0;
  unsigned long currentTime = millis();
  
  if(displayMode==0 && (currentTime - lastSlowUpdate >= 50)){
    updateEnergyAndState();
    checkOutputReal();
    updateDisplay1();
    sendOutputState();
    lastSlowUpdate = currentTime;
  }
if(displayMode==1 && (currentTime - lastSlowUpdate >= 500)){  // 20Hz for slower tasks
    updateEnergyAndState();
    checkSetpointEcho();
    updateDisplay2();
    sendOutputState();
    lastSlowUpdate = currentTime;
  }
  
  //delay(1);  // Very short delay for encoder responsiveness
}
