# STM32 Drivers Project

A collection of low-level hardware abstraction layer (HAL) drivers for the STM32F103C8 (Blue Pill) microcontroller, developed using the ARM GCC toolchain.

## Features

This project provides a set of modular drivers for common peripherals:

- **RCC**: Reset and Clock Control for system clock configuration.
- **GPIO**: General Purpose Input/Output for pin control.
- **UART**: Universal Asynchronous Receiver/Transmitter for serial communication.
- **I2C**: Inter-Integrated Circuit for communicating with external sensors and displays.
- **ADC**: Analog to Digital Converter for reading analog signals.
- **MPU6050**: Driver for the MPU6050 6-axis Accelerometer and Gyroscope.
- **OLED**: Driver for the SSD1306 OLED display.

## Project Structure

```text
.
├── CMSIS_library     # ARM CMSIS and STM32F1 headers
├── drivers           # Peripheral drivers
│   ├── adc           # ADC driver
│   ├── gpio          # GPIO driver
│   ├── i2c           # I2C driver
│   ├── mpu6050       # MPU6050 driver
│   ├── oled          # OLED (SSD1306) driver
│   ├── rcc           # RCC driver
│   └── uart          # UART driver
├── main.c            # Application entry point
├── Makefile          # Build configuration
├── linker.ld         # Linker script
└── startup.s         # Startup assembly code
```

## Getting Started

### Prerequisites

You will need the `arm-none-eabi-gcc` toolchain installed on your system.

### Building the Project

Use the provided Makefile to build the project:

```bash
make
```

This will generate the following files in the `build/` directory:
- `stm32f103c8.elf`
- `stm32f103c8.bin`
- `stm32f103c8.hex`

### Flashing the Device

To flash the binary to your STM32F103C8 using OpenOCD:

```bash
make flash
```

### Cleaning the Build

To remove all build artifacts:

```bash
make clean
```

## License

All rights reserved.
