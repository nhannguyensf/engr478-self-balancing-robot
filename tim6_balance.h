#ifndef __TIM6_BALANCE_H
#define __TIM6_BALANCE_H

#include <stdint.h>

// Global variable to store control loop time step (in seconds)
extern float control_dt;

// Initialize TIM6 to trigger balanceLoop at a specified frequency (Hz)
void TIM6_Init(uint32_t freq);

#endif // __TIM6_BALANCE_H
