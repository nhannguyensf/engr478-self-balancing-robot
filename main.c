// main.c - IMU test application
#include "stm32l476xx.h"
#include "i2c.h"
#include "imu.h"
#include "systick_timer.h"
#include "led.h"

int main(void)
{
    volatile float ax, ay, az, gx, gy, gz;

    initLED();          // Optional: Blink LED for activity
    initI2C1();         // Setup I2C1 (PB8=SCL, PB9=SDA)
    SysTick_Init(4000); // 1ms tick for 4MHz system clock
    initIMU();          // Wake up MPU6050
    test_I2C_MPU6050();

    while (1)
    {
        readAccelRaw(); // Update Ax, Ay, Az
        readGyroRaw();  // Update Gx, Gy, Gz

        // Assign to local variables for debugging
        ax = Ax;
        ay = Ay;
        az = Az;
        gx = Gx;
        gy = Gy;
        gz = Gz;

        toggleLED(); // Blink activity LED
        volatile int i;
        for (i = 0; i < 50000; i++)
            ;
    }
}
