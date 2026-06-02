# Makefile for STM32F103C8 build with debug symbols

# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

# Target
TARGET = stm32f103c8

# Build directory
BUILD_DIR = build

# Source files
C_SOURCES = main.c gpio/gpio.c uart/uart.c i2c/i2c.c mpu6050/mpu6050.c rcc/rcc.c adc/adc.c oled/oled_ssd1306.c
ASM_SOURCES = startup.s

# Object files (placed in build directory)
C_OBJECTS = $(addprefix $(BUILD_DIR)/, main.o gpio.o uart.o i2c.o mpu6050.o rcc.o adc.o oled_ssd1306.o)
ASM_OBJECTS = $(addprefix $(BUILD_DIR)/, startup.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Compiler flags
CFLAGS = -mcpu=cortex-m3 -mthumb -g -O0 -Wall -Wextra -std=c99
CFLAGS += $(addprefix -I, $(INC_DIRS))

# Linker flags
LDFLAGS = -T linker.ld -nostartfiles -Wl,--gc-sections --specs=nosys.specs

# Output files
ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = $(BUILD_DIR)/$(TARGET).bin
HEX = $(BUILD_DIR)/$(TARGET).hex

# Directories
SRC_DIR = .
INC_DIRS = . ./CMSIS_library ./gpio ./uart ./i2c ./mpu6050 ./rcc ./adc ./CMSIS_library/CMSIS ./oled

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Default target
all: $(BUILD_DIR) $(OBJECTS) $(BIN) $(HEX)

# Link ELF
$(ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Compile C sources
$(BUILD_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gpio.o: gpio/gpio.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/uart.o: uart/uart.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/i2c.o: i2c/i2c.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mpu6050.o: mpu6050/mpu6050.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/rcc.o: rcc/rcc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/adc.o: adc/adc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/oled_ssd1306.o: oled/oled_ssd1306.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM sources
$(BUILD_DIR)/startup.o: startup.s
	$(AS) -mcpu=cortex-m3 -mthumb -g -c $< -o $@

# Generate binary
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

# Generate hex
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Flash (assuming OpenOCD or similar, adjust as needed)
flash: $(BIN)
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(BIN) 0x08000000 verify reset exit"

# Debug (assuming GDB)
debug: $(ELF)
	arm-none-eabi-gdb $(ELF)

# Run in QEMU
run: $(ELF)
	qemu-system-arm -M stm32vldiscovery -kernel $(ELF) -nographic

.PHONY: all clean flash debug run
