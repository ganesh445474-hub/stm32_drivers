#include "../CMSIS_library/stm32f103xb.h"
#include "i2c.h"

#define I2C_TIMEOUT  10000

static uint8_t I2C_WaitSR1(uint32_t flag) {
    uint32_t t = I2C_TIMEOUT;
    while (!(I2C1->SR1 & flag)) {
        if (--t == 0) {
            I2C1->CR1 |= I2C_CR1_SWRST;
            I2C1->CR1 &= ~I2C_CR1_SWRST;
            return 0;
        }
    }
    return 1;
}

static uint8_t I2C_WaitBusy(void) {
    uint32_t t = I2C_TIMEOUT;
    while (I2C1->SR2 & I2C_SR2_BUSY) {
        if (--t == 0) return 0;
    }
    return 1;
}

static void repeated_start(void) {
    I2C1->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
    I2C_WaitSR1(I2C_SR1_SB);
}

void I2C1_Init(void) {
    // Enable GPIOB and I2C1 clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // PB6 = SCL, PB7 = SDA → AF Open-Drain, 50MHz
    GPIOB->CRL &= ~(0xFF << 24);
    GPIOB->CRL |=  (0xFF << 24);  // 0b1111_1111 for PB6 & PB7

    // Software reset
    I2C1->CR1 |=  I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // APB1 = 36 MHz
    I2C1->CR2 = 36;

    // Standard Mode 100 kHz: CCR = 36MHz / (2 * 100kHz) = 180
    I2C1->CCR = 180;

    // TRISE = (1000ns / (1/36MHz)) + 1 = 37
    I2C1->TRISE = 37;

    // Enable peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_Start(void) {
    I2C_WaitBusy();
    I2C1->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
    I2C_WaitSR1(I2C_SR1_SB);
}

void I2C1_Stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_SendAddress(uint8_t address, uint8_t direction) {
    I2C1->DR = (address << 1) | (direction & 0x01);
    I2C_WaitSR1(I2C_SR1_ADDR);
    // Clear ADDR by reading SR1 then SR2
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

void I2C1_WriteData(uint8_t *data, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        I2C_WaitSR1(I2C_SR1_TXE);
        I2C1->DR = data[i];
    }
    I2C_WaitSR1(I2C_SR1_BTF);
}

void I2C1_ReadData(uint8_t *buffer, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        if (i == length - 1) {
            I2C1->CR1 &= ~I2C_CR1_ACK; // NACK last byte
        }
        I2C_WaitSR1(I2C_SR1_RXNE);
        buffer[i] = (uint8_t)I2C1->DR;
    }
}

void I2C1_MasterWrite(uint8_t slaveAddr, uint8_t reg, uint8_t *data, uint8_t len) {
    I2C1_Start();
    I2C1_SendAddress(slaveAddr, 0);         // Write mode
    I2C_WaitSR1(I2C_SR1_TXE);
    I2C1->DR = reg;                         // Send register address
    I2C1_WriteData(data, len);
    I2C1_Stop();
}

void I2C1_MasterRead(uint8_t slaveAddr, uint8_t reg, uint8_t *buf, uint8_t len) {
    // Write register address
    I2C1_Start();
    I2C1_SendAddress(slaveAddr, 0);
    I2C_WaitSR1(I2C_SR1_TXE);
    I2C1->DR = reg;
   I2C_WaitSR1(I2C_SR1_BTF);
    
    // Repeated START → switch to read
    
    repeated_start();
    I2C1_SendAddress(slaveAddr, 1);         // Read mode
    I2C1_ReadData(buf, len);
    I2C1_Stop();
}