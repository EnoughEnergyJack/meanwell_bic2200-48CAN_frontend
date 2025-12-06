# Meanwell Bench Power Supply - User Guide

## Overview
This is a bench power supply controller with a 16x4 character LCD display. You can set voltage and current limits, monitor output, and track energy consumption.

## Display Modes

### Main Display (Default)
The main display shows real-time measurements and status:

```
V: XX.XXV        sec:XXX
I: XX.XXA
P: XX.XXW        OP:ON
E: XXXWh         [  ]
```

- **V**: Output voltage (volts)
- **I**: Output current (amps)
- **P**: Power consumption (watts)
- **E**: Energy consumed (watt-hours)
- **sec**: Timer showing how long the output has been enabled (in seconds)
- **OP**: Output status (ON or OFF)
- **[ ]**: Communication indicator (blinks when receiving data)

### Limits Display
Press the **VIEW** button to switch to the limits display:

```
LIMITS          BUF
Vs : XX.X < XX.X
I+ : XX.X < XX.X
I- : XX.X < XX.X
```

- **Vs**: Voltage setpoint (the target voltage)
- **I+**: Positive current limit
- **I-**: Negative current limit
- **<**: Arrow indicating which value you're editing
- **BUF**: Buffer values (temporary values you adjust before setting)

## Controls

### Rotary Encoders

1. **Voltage Encoder** (left knob)
   - Rotate to adjust the voltage buffer value
   - Range: 38.0V to 65.0V
   - Adjusts in 0.1V steps

2. **Current Encoder** (right knob)
   - Rotate to adjust the current buffer value
   - Which current limit you're editing depends on the **CHNG I** button state:
     - **I+** (positive current): 0.0A to 45.0A
     - **I-** (negative current): -38.0A to -0.001A
   - Adjusts in 0.1A steps

### Buttons

1. **VIEW Button**
   - Switches between Main Display and Limits Display
   - Press to toggle between the two views

2. **CHNG I Button**
   - Switches between editing positive current (I+) and negative current (I-)
   - The arrow (**<**) on the display shows which current limit you're currently editing
   - Only visible in Limits Display mode

3. **SET BUF Button**
   - Applies the buffer values as the actual setpoints
   - Press this button to confirm your voltage and current settings
   - The setpoints are sent to the power supply after pressing this button
   - Only works in Limits Display mode

4. **OP TOGGLE Button**
   - Turns the output ON or OFF
   - When turning ON, the timer and energy meter reset to zero
   - When turning OFF, the timer pauses and retains its value
   - Works in both display modes

## Basic Operation

### Setting Voltage and Current Limits

1. Press **VIEW** to switch to Limits Display
2. Rotate the **Voltage Encoder** to set your desired voltage (38-65V)
3. Rotate the **Current Encoder** to set the positive current limit (0-45A)
4. Press **CHNG I** to switch to negative current editing
5. Rotate the **Current Encoder** to set the negative current limit (-38A to 0A)
6. Press **SET BUF** to apply these settings to the power supply
7. The display will briefly show confirmation, then return to Main Display if output is enabled

### Enabling Output

1. Set your desired voltage and current limits (see above)
2. Press **OP TOGGLE** to turn the output ON
3. The display will show "OUTPUT STATE : ON" briefly
4. Monitor voltage, current, power, and energy on the Main Display

### Monitoring Operation

- Watch the **Main Display** for real-time measurements
- The **timer** shows how long the output has been enabled
- The **energy meter (E)** tracks total energy consumption in watt-hours
- The **power (P)** shows instantaneous power draw

### Disabling Output

- Press **OP TOGGLE** to turn the output OFF
- The timer will pause and retain its value
- Energy consumption stops accumulating

## Tips

- The buffer values are temporary until you press **SET BUF**
- The timer and energy meter reset when you turn the output ON
- If output is enabled, the display automatically returns to Main Display after setting limits
- The communication indicator (**[ ]**) blinks to show the controller is receiving data from the power supply

