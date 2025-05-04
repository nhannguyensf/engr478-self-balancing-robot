// imu.c - MPU6050 IMU driver (register-level I2C)
#include "imu.h"
#include "i2c.h"
#include "led.h"
#include "stm32l476xx.h"

#define MPU6050_ADDR (0x68 << 1)
#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1 0x6B
#define SMPLRT_DIV 0x19
#define CONFIG_REG 0x1A
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43

int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
float Ax, Ay, Az;
float Gx, Gy, Gz;

void initIMU(void)
{
    I2C_Write(MPU6050_ADDR, PWR_MGMT_1, 0x00);       // Wake up
    I2C_Write(MPU6050_ADDR, SMPLRT_DIV, 0x07);       // Sample rate: 1 kHz
    I2C_Write(MPU6050_ADDR, CONFIG_REG, 0x00);       // No DLPF
    I2C_Write(MPU6050_ADDR, GYRO_CONFIG_REG, 0x00);  // ±250 dps
    I2C_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, 0x00); // ±2g
}

void readAccelRaw(void)
{
    uint8_t data[6];
    I2C_ReadBurst(MPU6050_ADDR, ACCEL_XOUT_H, data, 6);
    Accel_X_RAW = (int16_t)(data[0] << 8 | data[1]);
    Accel_Y_RAW = (int16_t)(data[2] << 8 | data[3]);
    Accel_Z_RAW = (int16_t)(data[4] << 8 | data[5]);
    Ax = Accel_X_RAW / 16384.0f;
    Ay = Accel_Y_RAW / 16384.0f;
    Az = Accel_Z_RAW / 16384.0f;
}

void readGyroRaw(void)
{
    uint8_t data[6];
    I2C_ReadBurst(MPU6050_ADDR, GYRO_XOUT_H, data, 6);
    Gyro_X_RAW = (int16_t)(data[0] << 8 | data[1]);
    Gyro_Y_RAW = (int16_t)(data[2] << 8 | data[3]);
    Gyro_Z_RAW = (int16_t)(data[4] << 8 | data[5]);
    Gx = Gyro_X_RAW / 131.0f;
    Gy = Gyro_Y_RAW / 131.0f;
    Gz = Gyro_Z_RAW / 131.0f;
}

void testIMU(void)
{
    initIMU();
    initLED();
    while (1)
    {
        readAccelRaw();
        readGyroRaw();

        // Blink LED to indicate active reading
        toggleLED();
        volatile int d;
        for (d = 0; d < 200000; d++)
            ; // crude delay between reads
    }
}
