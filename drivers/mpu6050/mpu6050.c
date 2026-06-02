#include "mpu6050.h"
#include "i2c.h"

// Sensitivity scale factors (datasheet Table 1 & 2)
static float accel_scale = 16384.0f;  // LSB/g  for ±2g
static float gyro_scale  = 131.0f;    // LSB/°/s for ±250°/s

// Calibration offsets
int16_t accel_offset[3] = {0, 0, 0};
int16_t gyro_offset[3]  = {0, 0, 0};

/* ──────────────────────────────────────────────
   Initialize MPU6050
   ────────────────────────────────────────────── */
uint8_t MPU6050_Init(AccelRange accelRange, GyroRange gyroRange) {
    uint8_t data;

    // 1. Check WHO_AM_I
    I2C1_MasterRead(MPU6050_ADDR, MPU6050_WHO_AM_I, &data, 1);
    if (data != 0x68) return 0;  // Device not found

    // 2. Wake up: clear SLEEP bit in PWR_MGMT_1, use PLL with X gyro
    data = 0x01;  // CLKSEL = PLL with X-axis gyro (recommended)
    I2C1_MasterWrite(MPU6050_ADDR, MPU6050_PWR_MGMT_1, &data, 1);

    // 3. Set sample rate: SMPLRT_DIV = 7 → 1kHz / (1+7) = 125 Hz
    data = 0x07;
    I2C1_MasterWrite(MPU6050_ADDR, MPU6050_SMPLRT_DIV, &data, 1);

    // 4. Set DLPF: CONFIG = 0x01 → 184 Hz BW, ~2ms delay
    data = 0x01;
    I2C1_MasterWrite(MPU6050_ADDR, MPU6050_CONFIG, &data, 1);

    // 5. Set Gyro full-scale range
    data = (uint8_t)gyroRange;
    I2C1_MasterWrite(MPU6050_ADDR, MPU6050_GYRO_CFG, &data, 1);

    // Update gyro sensitivity
    switch (gyroRange) {
        case GYRO_RANGE_250:  gyro_scale = 131.0f;  break;
        case GYRO_RANGE_500:  gyro_scale = 65.5f;   break;
        case GYRO_RANGE_1000: gyro_scale = 32.8f;   break;
        case GYRO_RANGE_2000: gyro_scale = 16.4f;   break;
    }

    // 6. Set Accel full-scale range
    data = (uint8_t)accelRange;
    I2C1_MasterWrite(MPU6050_ADDR, MPU6050_ACCEL_CFG, &data, 1);

    // Update accel sensitivity
    switch (accelRange) {
        case ACCEL_RANGE_2G:  accel_scale = 16384.0f; break;
        case ACCEL_RANGE_4G:  accel_scale = 8192.0f;  break;
        case ACCEL_RANGE_8G:  accel_scale = 4096.0f;  break;
        case ACCEL_RANGE_16G: accel_scale = 2048.0f;  break;
    }

    return 1;  // Success
}

/* ──────────────────────────────────────────────
   Read all 14 raw bytes in one burst
   (ACCEL_X, ACCEL_Y, ACCEL_Z, TEMP, GYRO_X, GYRO_Y, GYRO_Z)
   ────────────────────────────────────────────── */
void MPU6050_ReadRaw(MPU6050_RawData *raw) {
    uint8_t buf[14];

    // Burst read 14 bytes starting at ACCEL_XOUT_H (0x3B)

    I2C1_MasterRead(MPU6050_ADDR, MPU6050_ACCEL_XOUT, buf, 14);
   

    // Combine high and low bytes (big-endian)
    raw->accel_x = (int16_t)(buf[0]  << 8 | buf[1]);
    raw->accel_y = (int16_t)(buf[2]  << 8 | buf[3]);
    raw->accel_z = (int16_t)(buf[4]  << 8 | buf[5]);
    raw->temp    = (int16_t)(buf[6]  << 8 | buf[7]);
    raw->gyro_x  = (int16_t)(buf[8]  << 8 | buf[9]);
    raw->gyro_y  = (int16_t)(buf[10] << 8 | buf[11]);
    raw->gyro_z  = (int16_t)(buf[12] << 8 | buf[13]);

    // Apply calibration offsets
    raw->accel_x -= accel_offset[0];
    raw->accel_y -= accel_offset[1];
    raw->accel_z -= accel_offset[2];
    raw->gyro_x  -= gyro_offset[0];
    raw->gyro_y  -= gyro_offset[1];
    raw->gyro_z  -= gyro_offset[2];
}

/* ──────────────────────────────────────────────
   Convert raw values to physical units
   ────────────────────────────────────────────── */
void MPU6050_ReadProcessed(MPU6050_Data *data) {
    MPU6050_RawData raw;
    MPU6050_ReadRaw(&raw);

    data->accel_x = (float)raw.accel_x / accel_scale;   // g
    data->accel_y = (float)raw.accel_y / accel_scale;
    data->accel_z = (float)raw.accel_z / accel_scale;

    data->gyro_x  = (float)raw.gyro_x  / gyro_scale;    // °/s
    data->gyro_y  = (float)raw.gyro_y  / gyro_scale;
    data->gyro_z  = (float)raw.gyro_z  / gyro_scale;

    // Temperature formula from datasheet:
    // Temp(°C) = (TEMP_OUT / 340.0) + 36.53
    data->temp = ((float)raw.temp / 340.0f) + 36.53f;
}

/* ──────────────────────────────────────────────
   Read temperature only
   ────────────────────────────────────────────── */
float MPU6050_GetTemp(void) {
    uint8_t buf[2];
    I2C1_MasterRead(MPU6050_ADDR, MPU6050_TEMP_OUT, buf, 2);
    int16_t raw_temp = (int16_t)(buf[0] << 8 | buf[1]);
    return ((float)raw_temp / 340.0f) + 36.53f;
}

/* ──────────────────────────────────────────────
   Calibration: average N samples at rest
   Device must be flat and stationary!
   ────────────────────────────────────────────── */
void MPU6050_Calibrate(uint16_t samples) {
    int32_t ax = 0, ay = 0, az = 0;
    int32_t gx = 0, gy = 0, gz = 0;
    MPU6050_RawData raw;

    // Reset offsets first
    for (int i = 0; i < 3; i++) {
        accel_offset[i] = 0;
        gyro_offset[i]  = 0;
    }

    for (uint16_t i = 0; i < samples; i++) {
        MPU6050_ReadRaw(&raw);
        ax += raw.accel_x;
        ay += raw.accel_y;
        az += raw.accel_z;
        gx += raw.gyro_x;
        gy += raw.gyro_y;
        gz += raw.gyro_z;
        for (volatile int d = 0; d < 1000; d++); // small delay
    }

    accel_offset[0] = ax / samples;
    accel_offset[1] = ay / samples;
    // Z-axis offset: subtract 1g (sensor reads ~16384 at rest for ±2g)
    accel_offset[2] = (az / samples) - (int16_t)accel_scale;

    gyro_offset[0]  = gx / samples;
    gyro_offset[1]  = gy / samples;
    gyro_offset[2]  = gz / samples;
}