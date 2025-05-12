#ifndef __SELF_BALANCE_H
#define __SELF_BALANCE_H

// ------------------------------------------------------------
// Self-Balancing Robot Control Header
// ------------------------------------------------------------

// Main control loop function to keep the robot balanced.
// This should be called at a fixed interval (e.g., 200 Hz using TIM6 interrupt).
void balanceLoop(void);

#endif // __SELF_BALANCE_H
