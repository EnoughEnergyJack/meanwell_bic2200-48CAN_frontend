# Meanwell Power Supply Front-End Controller

A user-friendly front-end controller for Meanwell programmable power supplies, providing intuitive voltage and current control via rotary encoders, real-time monitoring, and CAN bus communication.

## Features

- **Display Modes**
  - Main display: Real-time voltage, current, power, and energy monitoring
  - Limits display: Voltage and current setpoint configuration
  
- **Controls**
  - Rotary encoders for voltage (38-65V) and current (0-45A positive, -38A to 0A negative) adjustment
  - Dedicated buttons for output control, view switching, and setpoint confirmation
  
- **Real-Time Monitoring**
  - Output voltage and current readings
  - Power consumption (watts)
  - Energy consumption tracking (watt-hours)
  - Runtime timer
  - Output status indicator
  
- **CAN Bus Communication**
  - Bidirectional communication with Meanwell power supply
  - Automatic setpoint echo verification
  - Real-time data polling

## Hardware

The project uses a dedicated PCB designed specifically for the Arduino Uno. The PCB integrates all required hardware components:

- **16x4 Character LCD Display** (HD44780-compatible)
- **2x Rotary Encoders** with integrated push buttons (voltage and current adjustment)
- **4x Push Buttons** (VIEW, CHNG I, SET BUFFER, OUTPUT)
- **MCP2515 CAN Controller** with CAN transceiver
- **Power Input** (barrel jack connector)
- **Pin Headers** for Arduino Uno connection
- **CAN Bus Connector** for power supply communication

### Wiring Harness

For wiring harness details and external connections, refer to the schematic in the documentation:
- **Schematic**: See `Documentation/Meanwell_FE v17.pdf` for complete wiring diagrams and harness specifications

The PCB is designed to mount directly onto an Arduino Uno, with all components and connections pre-routed on the board.

## Software Dependencies

### Arduino Libraries
- `LiquidCrystal` - LCD display control (included with Arduino IDE)
- `mcp_can` - MCP2515 CAN controller library
- `SPI` - SPI communication (included with Arduino IDE)
- `Encoder` - Rotary encoder input handling

### Installation

1. Install the required libraries via Arduino Library Manager:
   ```
   - mcp_can (by Seeed Studio or compatible)
   - Encoder (by Paul Stoffregen)
   ```

2. Clone or download this repository.

3. Mount the dedicated PCB onto your Arduino Uno (see schematic for pin alignment)

4. Connect the wiring harness as specified in the schematic (`Documentation/Meanwell_FE v17.pdf`)

5. Open `Meanwell/Meanwell.ino` in the Arduino IDE

6. Configure pin assignments in `Meanwell/config.h` if needed (defaults should work with the PCB)

7. Upload the firmware to your Arduino board

## Pin Configuration

Default pin assignments (defined in `config.h`):

| Component | Pin | Description |
|-----------|-----|-------------|
| LCD RS | A2 | Register select |
| LCD EN | A3 | Enable |
| LCD D4-D7 | 5, 4, 3, A4 | Data lines (4-bit mode) |
| Voltage Encoder CLK | 6 | Clock pin |
| Voltage Encoder DT | 7 | Data pin |
| Current Encoder CLK | A0 | Clock pin |
| Current Encoder DT | 9 | Data pin |
| SET BUF Button | A1 | Set buffer values |
| OP TOGGLE Button | A5 | Output enable/disable |
| VIEW Button | 8 | Display mode toggle |
| CHNG I Button | 2 | Current direction toggle |
| CAN CS | 10 | CAN controller chip select |

## Usage

### Basic Operation

1. **Setting Voltage and Current Limits**
   - Press VIEW to enter Limits Display mode
   - Rotate voltage encoder to set desired voltage (38-65V)
   - Rotate current encoder to set positive current limit (0-45A)
   - Press CHNG I to switch to negative current editing
   - Rotate current encoder to set negative current limit (-38A to 0A)
   - Press SET BUF to apply settings to the power supply

2. **Enabling Output**
   - Configure desired limits (see above)
   - Press OP TOGGLE to enable output
   - Monitor real-time values on Main Display

3. **Monitoring**
   - Main Display shows:
     - Output voltage (V)
     - Output current (A)
     - Power consumption (W)
     - Energy consumed (Wh)
     - Runtime timer (seconds)
     - Output status (ON/OFF)

For detailed user instructions, see [USER_GUIDE.md](USER_GUIDE.md).

## Technical Details

### CAN Communication

- **Baud Rate**: 250 kbps
- **CAN ID TX**: 0xC0300 (commands to power supply)
- **CAN ID RX**: 0x800C0200 (responses from power supply)
- **Protocol**: PMBus-compatible commands over CAN
- **Mechanism**: Request-response pattern (see [CAN Messaging Protocol](Documentation/CAN_MESSAGING.md) for details)

- Voltage setpoint (CMD_VOUT_SET: 0x20)
- Current setpoint (CMD_IOUT_SET: 0x30)
- Reverse current setpoint (CMD_REVERSE_IOUT_SET: 0x130)
- Output enable/disable (CMD_OPERATION: 0x00)
- Read voltage (CMD_READ_VOUT: 0x60)
- Read current (CMD_READ_IOUT: 0x61)

### Display Update Rates

- Main Display: 20 Hz (50ms intervals)
- Limits Display: 2 Hz (500ms intervals)
- CAN polling: 10 Hz (100ms intervals)

### Energy Tracking

- Energy meter resets when output is enabled
- Power calculation uses filtered values for stability
- Energy integration: `E = ∫ P dt` (watt-hours)

## Development

### Building

1. Ensure all required libraries are installed
2. Open `Meanwell/Meanwell.ino` in Arduino IDE
3. Select your board and port
4. Compile and upload

### Debugging

- Serial output available at 115200 baud
- CAN communication status indicated by blinking `[ ]` on display
- Setpoint echo verification ensures commands are received

## Documentation

- [User Guide](USER_GUIDE.md) - Complete user manual
- [CAN Messaging Protocol](Documentation/CAN_MESSAGING.md) - Detailed explanation of the request-response CAN communication mechanism
- [Documentation/](Documentation/) - Hardware datasheets and specifications
- [MEANWELL.dbc](Documentation/MEANWELL.dbc) - CAN bus DBC file for protocol reference

## License

MIT


