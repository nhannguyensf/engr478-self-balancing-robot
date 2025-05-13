// self_balance.h
#ifndef SELF_BALANCE_H
#define SELF_BALANCE_H

#include <stdint.h>

/**
 * @brief  Calibrate gyro bias and prepare complementary filter.
 */
void calibrateGyro(void);

/**
 * @brief  Run balance control loop (called by TIM6 ISR).
 */
void balanceLoop(void);

// Tunable parameters (externally assignable)
extern float Kp;
extern float Ki;
extern float Kd;
extern float pitch_offset;

#endif // SELF_BALANCE_H
