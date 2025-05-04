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
    volatile int i;

    // Initial delay after power-up (~50ms @ 4MHz)
    // for (i = 0; i < 200000; i++)
    //     ;

    // Try to wake up MPU6050
    I2C_Write(MPU6050_ADDR, PWR_MGMT_1, 0x00);

    // Wait a bit for sensor to settle (~25ms)
    for (i = 0; i < 200000; i++)
        ;

    // Verify the wake-up
    uint8_t pwr = I2C_Read(MPU6050_ADDR, PWR_MGMT_1);
    if (pwr & 0x40) // If SLEEP bit is still set
    {
        // Retry one more time
        I2C_Write(MPU6050_ADDR, PWR_MGMT_1, 0x00);
        for (i = 0; i < 100000; i++)
            ;

        pwr = I2C_Read(MPU6050_ADDR, PWR_MGMT_1);
        if (pwr & 0x40)
        {
            while (1)
            {
                toggleLED(); // indicate failure
                for (i = 0; i < 100000; i++)
                    ;
            }
        }
    }

    // If wake successful, configure sensor
    I2C_Write(MPU6050_ADDR, SMPLRT_DIV, 0x07);
    I2C_Write(MPU6050_ADDR, CONFIG_REG, 0x00);
    I2C_Write(MPU6050_ADDR, GYRO_CONFIG_REG, 0x00);
    I2C_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, 0x00);
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

    uint8_t whoami = I2C_Read(MPU6050_ADDR, WHO_AM_I_REG);
    if (whoami != 0x68)
    {
        while (1)
        {
            toggleLED();
            volatile int i;
            for (i = 0; i < 100000; i++)
                ;
        }
    }

    while (1)
    {
        readAccelRaw();
        readGyroRaw();

        toggleLED();
        volatile int d;
        for (d = 0; d < 200000; d++)
            ;
    }
}
