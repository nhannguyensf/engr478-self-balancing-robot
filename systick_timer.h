#ifndef SYSTICK_TIMER_H
#define SYSTICK_TIMER_H

#include <stdint.h>

// Initialize SysTick timer for periodic interrupts
// ticks_per_sec: Number of desired SysTick interrupts per second (e.g., 1000 for 1ms resolution)
void SysTick_Init(uint32_t ticks_per_sec);

// Accurate non-blocking delay in milliseconds
void delay_ms(uint32_t T);

// Global millisecond counter (updated by SysTick_Handler)
extern volatile uint32_t msTicks;

#endif // SYSTICK_TIMER_H
