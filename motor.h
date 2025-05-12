#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

// ------------------------------------------------------------
// Motor Control for Self-Balancing Robot
// ------------------------------------------------------------

// Initialize motor control peripherals (GPIO, PWM via TIM2)
void initMotors(void);

// Drive LEFT motor with speed value from -1000 to 1000
// - Negative value: reverse direction
// - Positive value: forward direction
// - 0: Stop motor
void driveMotorLeft(int speed);

// Drive RIGHT motor with speed value from -1000 to 1000
// - Negative value: reverse direction
// - Positive value: forward direction
// - 0: Stop motor
void driveMotorRight(int speed);

#endif // __MOTOR_H
