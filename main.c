/* main.c */
#include "stm32l476xx.h"
#include "systick_timer.h"
#include "i2c.h"
#include "imu.h"
#include "motor.h"
#include "led.h"
#include "self_balance.h"
#include "tim6_balance.h"
#include "button.h"

extern volatile uint8_t program_running;

int main(void)
{
    // 1. Initialize Peripherals
    initLED();
    initI2C1();
    initMotors();
    initIMU();

    // 2. Calibrate Gyroscope
    calibrateGyro();

    // 3. Initialize SysTick
    SysTick_Init(1000); // 1ms ticks

    // 4. Configure User Button (PC13) to start/stop balancing
    Button_t userButton = {GPIOC, 13, 13, EXTI15_10_IRQn};
    configure_Button_interrupt(userButton);

    // 5. Initialize balance timer (TIM6) but do not start yet
    TIM6_Init(200); // Configure for 200Hz

    // 6. Main loop: wait for interrupts
    while (1)
    {
        __WFI(); // Low-power wait
    }
}
