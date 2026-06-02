# STM32F103C8 Drivers

A collection of low-level drivers for the STM32F103C8 microcontroller.

## Supported Hardware
- **MCU**: STM32F103C8 (Cortex-M3)
- **Peripherals**:
  - GPIO (General Purpose Input/Output)
  - UART (Universal Asynchronous Receiver/Transmitter)
  - I2C (Inter-Integrated Circuit)
  - ADC (Analog to Digital Converter)
  - RCC (Reset and Clock Control)

## External Device Drivers
- **OLED**: SSD1306 via I2C
- **IMU**: MPU6050 via I2C

## Project Structure
- `drivers/`: Core peripheral and external device drivers.
- `CMSIS_library/`: ARM CMSIS and STM32F1 standard peripheral library headers.
- `main.c`: Entry point and example usage.
- `linker.ld`: Linker script for the STM32F103C8 memory layout.
- `startup.s`: Startup assembly code.
