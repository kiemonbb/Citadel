# I2C Driver Test

## Purpose

Verify I2C driver works - register write, register read and correct START/STOP sequencing, using 24AA01 EEPROM.

## How to run

1. Power board via USB
2. Connect logic analyzer and 24AA01 EEPROM: SCL (PA9), SDA (PA10), GND
3. Run `make flash` at commit `94645fe` from the console
4. Observe I2C transmission in Logic with I2C analyzer

## Hardware setup

- EEPROM: 24AA01
- VCC -> 3.3V
- GND -> GND
- SDA -> PA10
- SCL -> PA9
- WP -> GND
- A0, A1, A2 -> left floating

## Result: PASS

Date: 12-05-2026
Notes:

- Write: `START [0x50 W] [0x00] [0xAA] [0xBB] [0xCC] [0xDD] STOP` - all bytes ACKed correctly
- Read: `START [0x50 W] [0x00] STOP START [0x50 R] [0xAA] [0xBB] [0xCC] [0xDD] STOP` - 4 bytes read back correctly every ~5s
