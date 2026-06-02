#ifndef I2C_H
#define I2C_H

#include "stm32f103xb.h"
#include <stdint.h>

void     I2C1_Init(void);
void     I2C1_Start(void);
void     I2C1_Stop(void);
void     I2C1_SendAddress(uint8_t address, uint8_t direction);
void     I2C1_WriteData(uint8_t *data, uint8_t length);
void     I2C1_ReadData(uint8_t *buffer, uint8_t length);
void     I2C1_MasterWrite(uint8_t slaveAddr, uint8_t reg, uint8_t *data, uint8_t len);
void     I2C1_MasterRead(uint8_t slaveAddr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif