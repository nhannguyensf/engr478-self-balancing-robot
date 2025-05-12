#include "stm32l476xx.h"
#include "self_balance.h"
#include "led.h" // Include this if using toggleLED()

// ------------------------------------------------------------
// TIM6 Initialization for Periodic Interrupt
// freq: Desired frequency in Hz to trigger balanceLoop
// Example: freq = 200 for 200 Hz (5 ms interval)
// ------------------------------------------------------------
void TIM6_Init(uint32_t freq)
{
    // 1. Enable TIM6 Clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

    uint32_t timer_clock = 4000000; // 4 MHz system clock
    uint32_t prescaler = 3999;      // Prescaler to get 1 ms timer tick (4 MHz / (3999+1) = 1000 Hz)
    TIM6->PSC = prescaler;

    // 2. Calculate ARR for Desired Frequency
    // Timer tick = 1 ms, so ARR = (1000 Hz / freq) - 1
    uint32_t arr_value = (timer_clock / ((prescaler + 1) * freq)) - 1;

    // Safety: Ensure ARR is at least 1
    if (arr_value < 1)
        arr_value = 1;

    TIM6->ARR = arr_value;

    // 3. Enable Update Interrupt
    TIM6->DIER |= TIM_DIER_UIE;

    // 4. Enable TIM6 and Auto-Reload Preload
    TIM6->CR1 |= TIM_CR1_ARPE; // Enable ARR preload buffer
    TIM6->CR1 |= TIM_CR1_CEN;  // Start Timer

    // 5. Enable TIM6 Interrupt in NVIC
    NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

// ------------------------------------------------------------
// TIM6 Interrupt Handler - Called on Timer Overflow
// ------------------------------------------------------------
void TIM6_DAC_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_UIF)
    {                            // Check Update Interrupt Flag
        TIM6->SR &= ~TIM_SR_UIF; // Clear Interrupt Flag
        toggleLED();             // Debugging: LED should blink if interrupt runs
        balanceLoop();           // Call Balance Control Loop
    }
}
