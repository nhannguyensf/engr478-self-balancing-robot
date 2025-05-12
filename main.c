#include "stm32l476xx.h"
#include "systick_timer.h"
#include "i2c.h"
#include "imu.h"
#include "motor.h"
#include "led.h"
#include "self_balance.h"
#include "tim6_balance.h" // Include TIM6 control

int main(void)
{
    // 1. Initialize Peripherals
    initLED();
    initI2C1();
    initMotors();
    initIMU();

    // 2. Calibrate Gyroscope Bias (Important for angle stability)
    calibrateGyro();

    // 3. Initialize SysTick for delay_ms() functionality
    SysTick_Init(1000); // 1 ms tick resolution

    // 4. Start TIM6 to trigger balanceLoop at desired frequency (e.g., 200 Hz)
    TIM6_Init(200); // 200 Hz control loop frequency

    // 5. Main loop enters low-power mode, control handled in interrupts
    while (1)
    {
        __WFI(); // Wait for Interrupt to minimize CPU usage
    }
}
