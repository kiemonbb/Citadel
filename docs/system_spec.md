# Product Requirements Document
- **Project Name:** Citadel
- **Version:** 1.0
- **Author:** Oktawian Bieszke - kiemonbb

## Executive Summary
- **Goals:** Create a low-power embedded wearable watch. The core focus is on providing precise timekeeping through a high accuracy RTC.
- **Success metric:** Device running for a month straight on a single charge.
## Hardware Specifications
- **MCU:** ARM Cortex-M series supporting low power modes ( preferably STM32L0xx ).
- **Display:** 1.3" SPI OLED or ideally E-paper for it's elegant look.
- **RTC:** Real-Time Clock with an average accuracy of ~2ppm able to work reliably in temperatures ranging from ~ -5°C to 35°C.
- **Power source:**  3.7 V ~150mAh Li-Ion/Li-Po accumulator .
## Functional Requirements
- **FR-01:** The system shall display time in HH:MM:SS format
- **FR-02:** The system shall maintain time with an average accuracy of roughly ~0.17 second/day ( ~1 minute/year )
-  **FR-03** The system shall retain clock state between power cycles.
-  **FR-04** The system shall have a button that makes the system show the time for about 5 seconds.
-  **FR-03** The system shall retain clock state between power cycle.
