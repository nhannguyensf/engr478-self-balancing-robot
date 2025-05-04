// systick_timer.h - Header for SysTick timer utilities
#ifndef SYSTICK_TIMER_H
#define SYSTICK_TIMER_H

#include <stdint.h>

extern volatile uint32_t msTicks; // Global time counter

void SysTick_Init(uint32_t Reload);
void delay(uint32_t T);

#endif
