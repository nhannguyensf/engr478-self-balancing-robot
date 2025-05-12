/* tim6_balance.c */
#include "stm32l476xx.h"
#include "self_balance.h"
#include "led.h"
#include "button.h" // for program_running

extern volatile uint8_t program_running; // Set by EXTI handler

// ------------------------------------------------------------
// TIM6 Initialization for Periodic Interrupt
// ------------------------------------------------------------
void TIM6_Init(uint32_t freq)
{
    // 1. Enable TIM6 Clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

    // 2. Configure for 1 ms ticks
    TIM6->PSC = 3999; // 4 MHz / (3999+1) = 1 kHz
    // 3. Compute ARR for desired loop rate (e.g. 200 Hz)
    uint32_t arr = (1000 / freq) - 1;
    TIM6->ARR = (arr < 1 ? 1 : arr);

    // 4. Enable Update Interrupt, Auto-Reload, and Start Timer
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;

    // 5. Enable IRQ in NVIC
    NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
    NVIC_SetPriority(TIM6_DAC_IRQn, 3);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

// ------------------------------------------------------------
// TIM6 Interrupt Handler – gates balanceLoop() on program_running
// ------------------------------------------------------------
void TIM6_DAC_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_UIF)
    {
        TIM6->SR &= ~TIM_SR_UIF; // clear the update flag

        // Only call balanceLoop when the button has started it
        if (program_running)
        {
            balanceLoop();
        }
    }
}
