# Meanwell Power Supply CAN Messaging Protocol

## Overview

The Meanwell power supply uses a **request-response** CAN bus communication protocol based on PMBus commands. Unlike typical CAN implementations where messages are broadcast, this system requires explicit requests to read values, and uses an echo mechanism to verify setpoints.

## CAN Bus Configuration

- **Baud Rate**: 250 kbps
- **CAN ID TX** (Controller → Power Supply): `0xC0300`
- **CAN ID RX** (Power Supply → Controller): `0xC0200`
- **Protocol**: PMBus-compatible commands over CAN

## Message Types

### 1. Read Requests (2-byte messages)

To read a value from the power supply, the controller sends a **2-byte request** containing only the command code:

**Request Format:**
```
Byte 0: Command code (LSB)
Byte 1: Command code (MSB)
```

**Response Format:**
```
Byte 0: Command code (LSB)
Byte 1: Command code (MSB)
Byte 2: Value (LSB) - in hundredths (0.01 units)
Byte 3: Value (MSB) - in hundredths (0.01 units)
```

**Example - Reading Voltage:**
```
Request:  [0x60, 0x00]  (CMD_READ_VOUT = 0x0060)
Response: [0x60, 0x00, 0xE8, 0x03]  (0x03E8 = 1000 → 10.00V)
```

**Example - Reading Current:**
```
Request:  [0x61, 0x00]  (CMD_READ_IOUT = 0x0061)
Response: [0x61, 0x00, 0x64, 0x00]  (0x0064 = 100 → 1.00A)
```

### 2. Write Commands (4-byte messages)

To set a value, the controller sends a **4-byte message** with the command and value:

**Write Format:**
```
Byte 0: Command code (LSB)
Byte 1: Command code (MSB)
Byte 2: Value (LSB) - in hundredths (0.01 units)
Byte 3: Value (MSB) - in hundredths (0.01 units)
```

**Example - Setting Voltage:**
```
Write: [0x20, 0x00, 0xE8, 0x03]  (CMD_VOUT_SET = 0x0020, 0x03E8 = 10.00V)
```

**Example - Setting Current:**
```
Write: [0x30, 0x00, 0x64, 0x00]  (CMD_IOUT_SET = 0x0030, 0x0064 = 1.00A)
```

### 3. Output Control (3-byte messages)

To enable/disable the output, the controller sends a **3-byte message**:

**Format:**
```
Byte 0: Command code (LSB) - CMD_OPERATION = 0x0000
Byte 1: Command code (MSB)
Byte 2: State (0 = OFF, 1 = ON)
```

**Example:**
```
Enable:  [0x00, 0x00, 0x01]
Disable: [0x00, 0x00, 0x00]
```

## Command Codes

| Command | Code (Hex) | Code (Dec) | Description |
|---------|------------|------------|-------------|
| `CMD_OPERATION` | 0x0000 | 0 | Output enable/disable |
| `CMD_VOUT_SET` | 0x0020 | 32 | Set voltage setpoint |
| `CMD_IOUT_SET` | 0x0030 | 48 | Set positive current setpoint |
| `CMD_FAULT_STATUS` | 0x0040 | 64 | Read fault status |
| `CMD_READ_VIN` | 0x0050 | 80 | Read input voltage |
| `CMD_READ_VOUT` | 0x0060 | 96 | Read output voltage |
| `CMD_READ_IOUT` | 0x0061 | 97 | Read output current |
| `CMD_READ_FAN_SPEED1` | 0x0070 | 112 | Read fan speed 1 |
| `CMD_READ_FAN_SPEED2` | 0x0071 | 113 | Read fan speed 2 |
| `CMD_SCALING_FACTOR` | 0x00C0 | 192 | Scaling factor |
| `CMD_SYSTEM_STATUS` | 0x00C1 | 193 | System status |
| `CMD_SYSTEM_CONFIG` | 0x00C2 | 194 | System configuration |
| `CMD_DIRECTION_CTRL` | 0x0100 | 256 | Direction control |
| `CMD_REVERSE_VOUT_SET` | 0x0120 | 288 | Set reverse voltage |
| `CMD_REVERSE_IOUT_SET` | 0x0130 | 304 | Set negative current |
| `CMD_BIDIRECTIONAL_CONFIG` | 0x0140 | 320 | Bidirectional configuration |

## Value Encoding

All values are encoded as **16-bit integers in hundredths** (0.01 units):

- **Voltage**: 10.00V = `0x03E8` (1000 in decimal)
- **Current**: 1.00A = `0x0064` (100 in decimal)
- **Power**: Calculated from V × I (not directly read)

**Encoding Formula:**
```cpp
uint16_t encoded = (uint16_t)(value * 100);
byte lsb = encoded & 0xFF;
byte msb = (encoded >> 8) & 0xFF;
```

**Decoding Formula:**
```cpp
uint16_t encoded = (rxBuf[3] << 8) | rxBuf[2];
float value = encoded * 0.01;
```

## Timing Considerations

### Delays Between Messages

The code includes strategic delays to ensure proper communication:

- **After setpoint writes**: 50ms delay between voltage, positive current, and negative current
- **After output state**: 10ms delay
- **Echo timeout**: 50ms maximum wait for response

### Polling Intervals

- **Real-time values**: 100ms (10 Hz) - alternates V/I
- **Setpoint echo**: 100ms (10 Hz) - cycles through all setpoints
- **Display updates**: 50ms (20 Hz) for main display, 500ms (2 Hz) for limits display

## Message Flow Examples

### Example 1: Setting Voltage and Current Limits

```
1. Controller sends: [0x20, 0x00, 0xE8, 0x03]  (Set 10.00V)
   Delay 50ms
   
2. Controller sends: [0x30, 0x00, 0x64, 0x00]  (Set 1.00A positive)
   Delay 50ms
   
3. Controller sends: [0x30, 0x01, 0x32, 0x00]  (Set 0.50A negative)
   
4. Controller requests echo: [0x20, 0x00]
   Power supply responds: [0x20, 0x00, 0xE8, 0x03]  ✓ Verified
   
5. Controller requests echo: [0x30, 0x00]
   Power supply responds: [0x30, 0x00, 0x64, 0x00]  ✓ Verified
   
6. Controller requests echo: [0x30, 0x01]
   Power supply responds: [0x30, 0x01, 0x32, 0x00]  ✓ Verified
```

### Example 2: Reading Real-Time Values

```
Time 0ms:
  Controller sends: [0x60, 0x00]  (Request voltage)
  Power supply responds: [0x60, 0x00, 0xE8, 0x03]  (10.00V)

Time 100ms:
  Controller sends: [0x61, 0x00]  (Request current)
  Power supply responds: [0x61, 0x00, 0x64, 0x00]  (1.00A)

Time 200ms:
  Controller sends: [0x60, 0x00]  (Request voltage)
  Power supply responds: [0x60, 0x00, 0xE9, 0x03]  (10.01V)
  
...continues alternating...
```

### Example 3: Enabling Output

```
1. Controller sends: [0x00, 0x00, 0x01]  (Enable output)
   Delay 10ms
   
2. Controller continues polling for voltage/current to monitor output
```

## Why This Protocol is "Weird"

1. **Mixed Message Sizes**: 
   - Read requests: 2 bytes
   - Write commands: 4 bytes
   - Output control: 3 bytes
   - Responses: 4 bytes

2. **No Broadcast Updates**: The power supply never sends unsolicited messages. All communication is initiated by the controller.

## Troubleshooting

### No Response from Power Supply

1. Check CAN bus wiring (CAN_H, CAN_L, GND)
2. Verify CAN baud rate (250 kbps)
3. Check CAN ID filters
4. Verify power supply is powered and in CAN mode

### Setpoint Not Applied

1. Check echo verification - if echo doesn't match, setpoint wasn't accepted
2. Verify value is within power supply limits
3. Check for fault conditions
4. Ensure proper delays between setpoint messages

### Missing Updates

1. Verify polling is active (check `canPacketsReceived` counter)
2. Check display update rates
3. Verify CAN receive buffer isn't full (code flushes after each read)

