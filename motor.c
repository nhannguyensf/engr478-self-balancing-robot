// motor.c - PWM motor control with direction using TIM2 and L298N
#include "motor.h"
#include "stm32l476xx.h"

#define MIN_EFFECTIVE_PWM 30 // Minimum PWM to avoid motor noise

// Initialize GPIOs and TIM2 for PWM-based motor control
void initMotors(void)
{
    // Enable clocks for GPIOA, GPIOB, and TIM2
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // Configure PB10 for TIM2_CH3 (PWM for Motor 1)
    GPIOB->MODER &= ~(0x3 << (2 * 10));
    GPIOB->MODER |= (0x2 << (2 * 10)); // Alternate function mode
    GPIOB->AFR[1] &= ~(0xF << (4 * 2));
    GPIOB->AFR[1] |= (1 << (4 * 2)); // AF1 for TIM2_CH3

    // Configure PA1 for TIM2_CH2 (PWM for Motor 2)
    GPIOA->MODER &= ~(0x3 << (2 * 1));
    GPIOA->MODER |= (0x2 << (2 * 1)); // Alternate function mode
    GPIOA->AFR[0] &= ~(0xF << (4 * 1));
    GPIOA->AFR[0] |= (1 << (4 * 1)); // AF1 for TIM2_CH2

    // Configure GPIO for motor direction control pins
    // PB4 = IN1, PB5 = IN2 for Motor 1
    GPIOB->MODER &= ~(0xF << (2 * 4));
    GPIOB->MODER |= (0x5 << (2 * 4)); // PB4 and PB5 as output

    // PA4 = IN3 for Motor 2
    GPIOA->MODER &= ~(0x3 << (2 * 4));
    GPIOA->MODER |= (0x1 << (2 * 4)); // PA4 as output

    // PB0 = IN4 for Motor 2
    GPIOB->MODER &= ~(0x3);
    GPIOB->MODER |= (0x1); // PB0 as output

    // Output type: Push-pull
    GPIOB->OTYPER &= ~(0x31);

    // Initialize direction pins to LOW (motors off)
    GPIOB->ODR &= ~(0x31);
    GPIOA->ODR &= ~(1 << 4);

    // Setup TIM2 for PWM at 5kHz frequency
    TIM2->PSC = 79;                              // Prescaler: 80 → 1MHz timer clock
    TIM2->ARR = 199;                             // Auto-reload value for 5kHz PWM frequency
    TIM2->CCR2 = 0;                              // Initialize duty cycle for Motor 2 to 0
    TIM2->CCR3 = 0;                              // Initialize duty cycle for Motor 1 to 0
    TIM2->CCMR1 |= (6 << 12);                    // PWM mode 1 on CH2 (Motor 2)
    TIM2->CCMR2 |= (6 << 4);                     // PWM mode 1 on CH3 (Motor 1)
    TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E; // Enable CH2 and CH3 outputs
    TIM2->CR1 |= TIM_CR1_CEN;                    // Start TIM2
}

// Drive motor with direction and speed control
void driveMotor(int side, int speed)
{
    int reverse = 0;

    // Determine direction based on sign of speed
    if (speed < 0)
    {
        reverse = 1;
        speed = -speed; // Take absolute value
    }

    // Clamp speed to allowable range
    if (speed > 1000)
        speed = 1000;
    if (speed > 0 && speed < MIN_EFFECTIVE_PWM)
        speed = MIN_EFFECTIVE_PWM; // Avoid low PWM causing noise

    if (side == MOTOR_LEFT)
    {
        // Control Motor 1 direction pins
        if (reverse)
        {
            GPIOB->ODR &= ~(1 << 4); // IN1 = 0
            GPIOB->ODR |= (1 << 5);  // IN2 = 1 → Reverse
        }
        else
        {
            GPIOB->ODR |= (1 << 4);  // IN1 = 1
            GPIOB->ODR &= ~(1 << 5); // IN2 = 0 → Forward
        }
        TIM2->CCR3 = speed; // Update PWM duty cycle for Motor 1
    }
    else if (side == MOTOR_RIGHT)
    {
        // Control Motor 2 direction pins
        if (reverse)
        {
            GPIOA->ODR &= ~(1 << 4); // IN3 = 0
            GPIOB->ODR |= (1 << 0);  // IN4 = 1 → Reverse
        }
        else
        {
            GPIOA->ODR |= (1 << 4);  // IN3 = 1
            GPIOB->ODR &= ~(1 << 0); // IN4 = 0 → Forward
        }
        TIM2->CCR2 = speed; // Update PWM duty cycle for Motor 2
    }
}

// Test Functions for Manual Motor Control

// Drive both motors forward at the specified speed
void forward(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, speed);
}

// Drive both motors backward at the specified speed
void backward(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

// Turn left by reversing left motor and driving right motor forward
void turnLeft(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, speed);
}

// Turn right by driving left motor forward and reversing right motor
void turnRight(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

// Simple motor test sequence to check motor functionality
void motorTest(void)
{
    volatile int i;
    forward(600);
    for (i = 0; i < 400000; i++)
        ;
    turnLeft(500);
    for (i = 0; i < 400000; i++)
        ;
    turnRight(500);
    for (i = 0; i < 400000; i++)
        ;
    backward(600);
    for (i = 0; i < 400000; i++)
        ;
    forward(0); // Stop both motors
}

// Test Motor 1 running forward for approximately 3 seconds
void testMotor1Forward(void)
{
    GPIOB->ODR |= (1 << 4);  // IN1 = 1
    GPIOB->ODR &= ~(1 << 5); // IN2 = 0
    TIM2->CCR3 = 1000;       // Full speed on PB10
    volatile int i;
    for (i = 0; i < 1200000; i++)
        ;
    TIM2->CCR3 = 0; // Stop motor
    GPIOB->ODR &= ~((1 << 4) | (1 << 5));
}

// Test Motor 2 running forward for approximately 3 seconds
void testMotor2Forward(void)
{
    GPIOA->ODR |= (1 << 4);  // IN3 = 1
    GPIOB->ODR &= ~(1 << 0); // IN4 = 0
    TIM2->CCR2 = 1000;       // Full speed on PA1
    volatile int i;
    for (i = 0; i < 1200000; i++)
        ;
    TIM2->CCR2 = 0; // Stop motor
    GPIOA->ODR &= ~(1 << 4);
    GPIOB->ODR &= ~(1 << 0);
}
