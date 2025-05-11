#include "stm32l476xx.h"
#include "systick_timer.h"
#include "self_balance.h" // Ensure balanceLoop is called from interrupt

#define SystemCoreClock 4000000 // 4 MHz system clock

volatile uint32_t msTicks = 0; // Global millisecond counter

// Initialize SysTick to generate periodic interrupts
// ticks_per_sec: Desired interrupt frequency (e.g., 1000 for 1ms resolution)
void SysTick_Init(uint32_t ticks_per_sec)
{
    SysTick->LOAD = (SystemCoreClock / ticks_per_sec) - 1; // Set reload value
    SysTick->VAL = 0;                                      // Clear current SysTick counter
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk             // Use processor clock (AHB)
                    | SysTick_CTRL_TICKINT_Msk             // Enable SysTick interrupt
                    | SysTick_CTRL_ENABLE_Msk;             // Enable SysTick timer
}

// SysTick Interrupt Handler - Executes balance loop and updates time counter
void SysTick_Handler(void)
{
    balanceLoop(); // Call the self-balancing control loop
    msTicks++;     // Increment millisecond counter
}

// Non-blocking, accurate delay in milliseconds
void delay_ms(uint32_t T)
{
    uint32_t start = msTicks;
    while ((msTicks - start) < T)
        ;
}
