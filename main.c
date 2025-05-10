// main.c - Self-balancing robot entry point using SysTick Interrupt Control

#include "stm32l476xx.h"
#include "systick_timer.h"
#include "i2c.h"
#include "imu.h"
#include "motor.h"
#include "led.h"
#include "self_balance.h"

int main(void)
{
    // Initialize system peripherals
    initLED();    // Optional: LED for debugging/status
    initI2C1();   // Initialize I2C for IMU communication
    initMotors(); // Configure PWM and direction control for motors
    initIMU();    // Wake up and configure MPU6050 IMU

    // Initialize SysTick to trigger balanceLoop at 200 Hz (5ms intervals)
    SysTick_Init(200);

    while (1)
    {
        // Main loop intentionally left empty
        // Balance control is executed inside SysTick_Handler interrupt
        __WFI(); // Enter low-power mode until next interrupt
    }
}
