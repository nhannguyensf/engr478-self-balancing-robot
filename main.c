// main.c - Self-balancing robot entry point
#include "stm32l476xx.h"
#include "systick_timer.h"
#include "i2c.h"
#include "imu.h"
#include "motor.h"
#include "led.h"
#include "self_balance.h"

int main(void)
{
    initLED();
    SysTick_Init(4000); // 1ms tick assuming 4 MHz
    initI2C1();
    initMotors();
    initIMU();

    while (1)
    {
        balanceLoop(); // Main control loop
    }
}
