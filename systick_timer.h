#ifndef __SYSTICK_TIMER_H
#define __SYSTICK_TIMER_H

#include <stdint.h>

// Initializes SysTick timer for periodic interrupts
// ticks_per_sec: Frequency of SysTick interrupts (e.g., 1000 for 1 ms tick)
void SysTick_Init(uint32_t ticks_per_sec);

// Non-blocking delay function using SysTick counter
// T: Delay in milliseconds
void delay_ms(uint32_t T);

extern volatile uint32_t msTicks; // Global time counter (increments every millisecond)

#endif // __SYSTICK_TIMER_H
