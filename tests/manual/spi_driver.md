# SPI Driver Test

## Purpose

Verify SPI driver works - data transmission, CS pin control and peripheral configuration

## How to run

1. Power board via USB
2. Connect logic analyzer: SCK (PB3), MOSI (PB5), CS (PB6), GND
3. Run `make flash` at commit `85740c0` from the console
4. Observe SPI transmission in Logic with SPI analyzer (CPOL=0, CPHA=0, CS=activelow)

## Result: PASS

Date: 03-05-2026
Notes: 4 bytes - 0xA5, 0xDE, 0x55, 0xAA - transmitted correctly every ~1s, CS asserts/deasserts cleanly and clock signal is stable
