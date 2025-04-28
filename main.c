// The Nucleo Team - ENGR 478 - Spring 2025
// main.c - Test program to read tilt angle from MPU6050

#include "stm32l476xx.h"
#include "i2c.h"
#include "imu.h"
#include "systick_timer.h"

int main(void)
{
    SysTick_Init(4000);
    initLED();
    initI2C1();
    initIMU();
    calibrateGyro();

    while (1)
    {
        volatile float tilt = getTiltAngle();

        volatile int i;
        for (i = 0; i < 1000; i++)
            ;
    }
}
