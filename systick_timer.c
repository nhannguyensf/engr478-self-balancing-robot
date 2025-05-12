#include "stm32l476xx.h"
#include "systick_timer.h"

#define SystemCoreClock 4000000 // 4 MHz system clock for STM32L476RG

volatile uint32_t msTicks = 0; // Global millisecond counter for delay and timing

// ------------------------------------------------------------
// SysTick Initialization
// ticks_per_sec: Desired SysTick interrupt frequency (e.g., 1000 for 1ms resolution)
// ------------------------------------------------------------
void SysTick_Init(uint32_t ticks_per_sec)
{
    SysTick->LOAD = (SystemCoreClock / ticks_per_sec) - 1; // Set reload value for desired tick rate
    SysTick->VAL = 0;                                      // Clear current SysTick counter value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk             // Use processor clock (AHB)
                    | SysTick_CTRL_TICKINT_Msk             // Enable SysTick interrupt
                    | SysTick_CTRL_ENABLE_Msk;             // Enable SysTick timer
}

// ------------------------------------------------------------
// SysTick Interrupt Handler
// Purpose: ONLY increments the msTicks counter for accurate delays.
// ------------------------------------------------------------
void SysTick_Handler(void)
{
    msTicks++; // Increment the millisecond counter
    // No heavy tasks here to avoid interrupt delay affecting timekeeping
}

// ------------------------------------------------------------
// Non-blocking Millisecond Delay Function
// T: Delay time in milliseconds
// Uses the msTicks counter updated by SysTick_Handler.
// ------------------------------------------------------------
void delay_ms(uint32_t T)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < T)
        ; // Wait until the specified time has passed
}
