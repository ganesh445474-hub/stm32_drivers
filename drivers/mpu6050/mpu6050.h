#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

// I2C Address
#define MPU6050_ADDR        0x68    // AD0 = LOW

// Registers
#define MPU6050_SMPLRT_DIV  0x19
#define MPU6050_CONFIG      0x1A
#define MPU6050_GYRO_CFG    0x1B
#define MPU6050_ACCEL_CFG   0x1C
#define MPU6050_ACCEL_XOUT  0x3B    // Burst read starts here
#define MPU6050_TEMP_OUT    0x41
#define MPU6050_GYRO_XOUT   0x43
#define MPU6050_PWR_MGMT_1  0x6B
#define MPU6050_WHO_AM_I    0x75

// Accel full-scale range
typedef enum {
    ACCEL_RANGE_2G  = 0x00,
    ACCEL_RANGE_4G  = 0x08,
    ACCEL_RANGE_8G  = 0x10,
    ACCEL_RANGE_16G = 0x18
} AccelRange;

// Gyro full-scale range
typedef enum {
    GYRO_RANGE_250  = 0x00,
    GYRO_RANGE_500  = 0x08,
    GYRO_RANGE_1000 = 0x10,
    GYRO_RANGE_2000 = 0x18
} GyroRange;

// Processed data struct
typedef struct {
    float accel_x, accel_y, accel_z;   // in g
    float gyro_x,  gyro_y,  gyro_z;    // in °/s
    float temp;                          // in °C
} MPU6050_Data;

// Raw data struct
typedef struct {
    int16_t accel_x, accel_y, accel_z;
    int16_t gyro_x,  gyro_y,  gyro_z;
    int16_t temp;
} MPU6050_RawData;

uint8_t MPU6050_Init(AccelRange accelRange, GyroRange gyroRange);
void    MPU6050_ReadRaw(MPU6050_RawData *raw);
void    MPU6050_ReadProcessed(MPU6050_Data *data);
float   MPU6050_GetTemp(void);
void    MPU6050_Calibrate(uint16_t samples);

extern int16_t accel_offset[3];
extern int16_t gyro_offset[3];

#endif