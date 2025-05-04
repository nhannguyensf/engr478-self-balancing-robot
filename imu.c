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

IMU_Data imu_data;

void initIMU(void)
{
    volatile int i;
    I2C_Write(MPU6050_ADDR, PWR_MGMT_1, 0x00);

    for (i = 0; i < 200000; i++)
        ;

    uint8_t pwr = I2C_Read(MPU6050_ADDR, PWR_MGMT_1);
    if (pwr & 0x40)
    {
        I2C_Write(MPU6050_ADDR, PWR_MGMT_1, 0x00);
        for (i = 0; i < 100000; i++)
            ;

        pwr = I2C_Read(MPU6050_ADDR, PWR_MGMT_1);
        if (pwr & 0x40)
        {
            while (1)
            {
                toggleLED();
                for (i = 0; i < 100000; i++)
                    ;
            }
        }
    }

    I2C_Write(MPU6050_ADDR, SMPLRT_DIV, 0x07);
    I2C_Write(MPU6050_ADDR, CONFIG_REG, 0x00);
    I2C_Write(MPU6050_ADDR, GYRO_CONFIG_REG, 0x00);
    I2C_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, 0x00);
}

void readIMU_AllRaw(void)
{
    uint8_t buffer[12];
    I2C_ReadBurst(MPU6050_ADDR, ACCEL_XOUT_H, buffer, 12);

    imu_data.acc_x_raw = (int16_t)(buffer[0] << 8 | buffer[1]);
    imu_data.acc_y_raw = (int16_t)(buffer[2] << 8 | buffer[3]);
    imu_data.acc_z_raw = (int16_t)(buffer[4] << 8 | buffer[5]);
    imu_data.gyro_x_raw = (int16_t)(buffer[6] << 8 | buffer[7]);
    imu_data.gyro_y_raw = (int16_t)(buffer[8] << 8 | buffer[9]);
    imu_data.gyro_z_raw = (int16_t)(buffer[10] << 8 | buffer[11]);

    imu_data.acc_x = imu_data.acc_x_raw / 16384.0f;
    imu_data.acc_y = imu_data.acc_y_raw / 16384.0f;
    imu_data.acc_z = imu_data.acc_z_raw / 16384.0f;

    imu_data.gyro_x = imu_data.gyro_x_raw / 131.0f;
    imu_data.gyro_y = imu_data.gyro_y_raw / 131.0f;
    imu_data.gyro_z = imu_data.gyro_z_raw / 131.0f;
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
        readIMU_AllRaw();
        toggleLED();
        volatile int d;
        for (d = 0; d < 200000; d++)
            ;
    }
}
