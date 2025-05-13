// main.c
#include "stm32l476xx.h"
#include "systick_timer.h"
#include "i2c.h"
#include "imu.h"
#include "motor.h"
#include "led.h"
#include "tim6_balance.h"
#include "button.h"
#include "ADC.h"
#include "self_balance.h"

extern volatile uint8_t program_running;

// Delta ranges for tuning (added to default values)
#define OFFSET_DELTA_MIN -5.0f
#define OFFSET_DELTA_MAX 5.0f
#define KP_DELTA_MIN -2.0f
#define KP_DELTA_MAX 2.0f
#define KI_DELTA_MIN -0.1f
#define KI_DELTA_MAX 0.1f
#define KD_DELTA_MIN -0.5f
#define KD_DELTA_MAX 0.5f

volatile uint32_t result[256];
uint32_t last_val;

int main(void)
{
    int i;

    // 1. Init clocks & peripherals
    initLED();
    initI2C1();
    initMotors();
    initIMU();

    // 2. ADC for tuning
    ADC_Init(); // config PA0, wakeup, common-cfg

    // 3. Calibrate gyro
    calibrateGyro();

    // 4. SysTick & button
    SysTick_Init(1000);
    Button_t userButton = {GPIOC, 13, 13, EXTI15_10_IRQn};
    configure_Button_interrupt(userButton);

    // 5. TIM6 for balance ISR
    TIM6_Init(200);

    // 6. Capture default base values
    const float base_offset = pitch_offset;
    const float base_Kp = Kp;
    const float base_Ki = Ki;
    const float base_Kd = Kd;

    i = 0; // buffer index

    // 7. Main idle/tuning loop
    while (1)
    {
        if (!program_running)
        {
            // -- raw reading for LED feedback and logging --
            uint16_t raw = ADC_Read();
            if (raw > 2048)
                onLED();
            else
                offLED();

            result[(i++ & 0x00FF)] = raw;
            last_val = raw;

            // Future gain tuning: (uncomment each one if needed)
            // pitch_offset = base_offset + ADC_ReadFloat(OFFSET_DELTA_MIN, OFFSET_DELTA_MAX);
            // Kp = base_Kp + ADC_ReadFloat(KP_DELTA_MIN, KP_DELTA_MAX);
            // Ki = base_Ki + ADC_ReadFloat(KI_DELTA_MIN, KI_DELTA_MAX);
            // Kd = base_Kd + ADC_ReadFloat(KD_DELTA_MIN, KD_DELTA_MAX);
        }

        __WFI(); // wait for interrupt
    }
}
