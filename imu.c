// imu.c - MPU6050 full sensor driver and tilt angle computation using msTicks
#include "stm32l476xx.h"
#include "imu.h"
#include "i2c.h"
#include "systick_timer.h"
#include <math.h>
#include "led.h"

#define MPU6050_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43

static float angle = 0.0f;
static float gyroBias = 0.0f;

extern volatile uint32_t msTicks; // Use global msTicks for timekeeping

void initIMU(void)
{
    I2C_Write(MPU6050_ADDR << 1, PWR_MGMT_1, 0x00); // Wake up MPU6050
}

static int16_t readWord(uint8_t reg)
{
    uint8_t high = I2C_Read(MPU6050_ADDR << 1, reg);
    uint8_t low = I2C_Read(MPU6050_ADDR << 1, reg + 1);
    return (int16_t)(high << 8 | low);
}

void calibrateGyro(void)
{
    int32_t sum = 0;
    int i;
    for (i = 0; i < 100; i++)  // Faster 100 samples
    {
        sum += readWord(GYRO_XOUT_H);

        if (i % 10 == 0) {
            toggleLED(); // Blink LED every 10 samples
        }
    }
    gyroBias = sum / 100.0f;

    offLED(); // Turn off LED after done
}



float getTiltAngle(void)
{
    static uint32_t lastTicks = 0;
    uint32_t now = msTicks;
    float dt = (now - lastTicks) * 1.0f; // 1 tick = 1 second based on 4MHz
    lastTicks = now;

    int16_t ax_raw = readWord(ACCEL_XOUT_H);
    int16_t az_raw = readWord(ACCEL_XOUT_H + 4);
    int16_t gx_raw = readWord(GYRO_XOUT_H);

    float accAngle = atan2f(ax_raw, az_raw) * 180.0f / 3.1415926f;
    float gyroRate = (gx_raw - gyroBias) / 131.0f; // degrees/sec

    // Complementary filter
    angle = 0.98f * (angle + gyroRate * dt) + 0.02f * accAngle;

    return angle;
}
