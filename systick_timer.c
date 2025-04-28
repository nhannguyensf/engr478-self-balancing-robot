// systick_timer.c - SysTick timer setup based on default 4 MHz clock
#include "stm32l476xx.h"
#include "systick_timer.h"

volatile uint32_t msTicks = 0; // Global time counter

void SysTick_Init(uint32_t Reload)
{
    // 1. Disable SysTick
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    // 2. Set reload value (Reload must be -1 because SysTick counts down to 0)
    SysTick->LOAD = Reload - 1;

    // 3. Clear current value
    SysTick->VAL = 0;

    // 4. Enable SysTick interrupt
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    // 5. Select processor clock
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    // 6. Enable SysTick
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

// SysTick Interrupt Handler
void SysTick_Handler(void)
{
    msTicks++; // Increment 1 every interrupt
}
