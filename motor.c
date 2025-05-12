// motor.c - PWM Motor Control with Direction using TIM2 and L298N H-Bridge

#include "motor.h"
#include "stm32l476xx.h"
#include "systick_timer.h" // For delay_ms()

#define MIN_EFFECTIVE_PWM 30 // Minimum effective PWM value (logical range: 0–1000)
#define PWM_MAX_INPUT 1000   // Logical max PWM input value
#define TIM_ARR_VALUE 199    // TIM2->ARR value (max counter for PWM)

#define SCALE_PWM(value) ((value) * TIM_ARR_VALUE / PWM_MAX_INPUT) // Scale logical PWM to timer count

// -------------------------------
// GPIO Pin Definitions
// -------------------------------
#define LEFT_IN1_PORT GPIOB
#define LEFT_IN1_PIN 4
#define LEFT_IN2_PORT GPIOB
#define LEFT_IN2_PIN 5
#define LEFT_PWM_PORT GPIOB
#define LEFT_PWM_PIN 10 // PB10 → TIM2_CH3 (Motor 1)

#define RIGHT_IN3_PORT GPIOA
#define RIGHT_IN3_PIN 4
#define RIGHT_IN4_PORT GPIOB
#define RIGHT_IN4_PIN 0
#define RIGHT_PWM_PORT GPIOA
#define RIGHT_PWM_PIN 1 // PA1 → TIM2_CH2 (Motor 2)

// -------------------------------
// Motor Initialization
// -------------------------------
void initMotors(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // LEFT Motor PWM - PB10 (AF1)
    LEFT_PWM_PORT->MODER &= ~(0x3 << (2 * LEFT_PWM_PIN));
    LEFT_PWM_PORT->MODER |= (0x2 << (2 * LEFT_PWM_PIN));
    LEFT_PWM_PORT->AFR[1] &= ~(0xF << (4 * (LEFT_PWM_PIN - 8)));
    LEFT_PWM_PORT->AFR[1] |= (1 << (4 * (LEFT_PWM_PIN - 8)));

    // RIGHT Motor PWM - PA1 (AF1)
    RIGHT_PWM_PORT->MODER &= ~(0x3 << (2 * RIGHT_PWM_PIN));
    RIGHT_PWM_PORT->MODER |= (0x2 << (2 * RIGHT_PWM_PIN));
    RIGHT_PWM_PORT->AFR[0] &= ~(0xF << (4 * RIGHT_PWM_PIN));
    RIGHT_PWM_PORT->AFR[0] |= (1 << (4 * RIGHT_PWM_PIN));

    // Configure Direction Pins
    GPIOB->MODER &= ~(0xF << (2 * 4));
    GPIOB->MODER |= (0x5 << (2 * 4));
    GPIOA->MODER &= ~(0x3 << (2 * RIGHT_IN3_PIN));
    GPIOA->MODER |= (0x1 << (2 * RIGHT_IN3_PIN));
    GPIOB->MODER &= ~(0x3);
    GPIOB->MODER |= (0x1);

    GPIOB->OTYPER &= ~(0x31);
    GPIOB->ODR &= ~(0x31);
    GPIOA->ODR &= ~(1 << RIGHT_IN3_PIN);

    TIM2->PSC = 79;            // 4 MHz / (79+1) = 50 kHz Timer Clock
    TIM2->ARR = TIM_ARR_VALUE; // 5 kHz PWM frequency

    TIM2->CCR2 = 0; // Right Motor
    TIM2->CCR3 = 0; // Left Motor

    TIM2->CCMR1 |= (6 << 12);
    TIM2->CCMR2 |= (6 << 4);
    TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E;
    TIM2->CR1 |= TIM_CR1_CEN;
}

// -------------------------------
// Motor Control Functions (Logical Range: -1000 to 1000)
// -------------------------------

void driveMotorLeft(int speed)
{
    int reverse = 0;

    if (speed < 0)
    {
        reverse = 1;
        speed = -speed;
    }

    if (speed > PWM_MAX_INPUT)
        speed = PWM_MAX_INPUT;
    if (speed > 0 && speed < MIN_EFFECTIVE_PWM)
        speed = MIN_EFFECTIVE_PWM;

    if (reverse)
    {
        LEFT_IN1_PORT->ODR |= (1 << LEFT_IN1_PIN); // Swapped
        LEFT_IN2_PORT->ODR &= ~(1 << LEFT_IN2_PIN);
    }
    else
    {
        LEFT_IN1_PORT->ODR &= ~(1 << LEFT_IN1_PIN); // Swapped
        LEFT_IN2_PORT->ODR |= (1 << LEFT_IN2_PIN);
    }

    TIM2->CCR3 = SCALE_PWM(speed);
}

void driveMotorRight(int speed)
{
    int reverse = 0;

    if (speed < 0)
    {
        reverse = 1;
        speed = -speed;
    }

    if (speed > PWM_MAX_INPUT)
        speed = PWM_MAX_INPUT;
    if (speed > 0 && speed < MIN_EFFECTIVE_PWM)
        speed = MIN_EFFECTIVE_PWM;

    if (reverse)
    {
        RIGHT_IN3_PORT->ODR |= (1 << RIGHT_IN3_PIN); // Swapped
        RIGHT_IN4_PORT->ODR &= ~(1 << RIGHT_IN4_PIN);
    }
    else
    {
        RIGHT_IN3_PORT->ODR &= ~(1 << RIGHT_IN3_PIN); // Swapped
        RIGHT_IN4_PORT->ODR |= (1 << RIGHT_IN4_PIN);
    }

    TIM2->CCR2 = SCALE_PWM(speed);
}
