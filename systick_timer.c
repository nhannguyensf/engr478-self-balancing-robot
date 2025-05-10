// systick_timer.c - SysTick timer setup with interrupt-based control
#include "stm32l476xx.h"
#include "systick_timer.h"

#define SystemCoreClock 4000000 // Manually define system clock as 4 MHz

volatile uint32_t msTicks = 0; // Global time counter (increments every SysTick interrupt)

// Initialize SysTick Timer for periodic interrupts
// ticks_per_sec: Number of desired SysTick interrupts per second
void SysTick_Init(uint32_t ticks_per_sec)
{
    SysTick->LOAD = (SystemCoreClock / ticks_per_sec) - 1; // Set reload value for desired frequency
    SysTick->VAL = 0;                                      // Clear current SysTick counter value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk             // Use processor clock (AHB)
                    | SysTick_CTRL_TICKINT_Msk             // Enable SysTick interrupt
                    | SysTick_CTRL_ENABLE_Msk;             // Enable SysTick timer
}

// SysTick Interrupt Handler (called at configured frequency)
// Directly calls balance control loop to maintain real-time control
void SysTick_Handler(void)
{
    balanceLoop(); // Execute self-balancing control loop
    msTicks++;     // Increment global millisecond counter
}

// Crude delay function (blocking delay)
// T: Desired delay time in milliseconds
void delay(uint32_t T)
{
    volatile uint32_t i;
    for (i = 0; i < T * 4000; i++)
        ; // Approximate delay loop for 4 MHz clock
}
