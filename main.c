// main.c - IMU test application using register-level I2C
#include "stm32l476xx.h"
#include "i2c.h"
#include "imu.h"
#include "systick_timer.h"
#include "led.h"

int main(void)
{
    initLED();          // LED for activity feedback
    initI2C1();         // Setup I2C1 on PB8/PB9
    SysTick_Init(4000); // 1ms system tick for 4MHz core
		
    testIMU();          // Start IMU read loop

    // Should never reach here
    while (1)
        ;
}
