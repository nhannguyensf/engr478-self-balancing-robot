// main.c - Entry point for motor test using TIM2 and GPIO direction
#include "stm32l476xx.h"
#include "motor.h"

int main(void)
{
    // Initialize system and motors
    initMotors();

    // Run predefined motor movement test sequence
    motorTest();

    // Loop forever
    while (1) {
        // Optional: insert user behavior or idle state
    }
}
