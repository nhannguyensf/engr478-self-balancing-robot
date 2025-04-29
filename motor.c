// motor.c - PWM motor control with full direction using L298N IN1–IN4
#include "motor.h"
#include "stm32l476xx.h"

void initMotors(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // PA8 = PWM Left (TIM1_CH1), PA9 = PWM Right (TIM1_CH2)
    GPIOA->MODER &= ~(0xF << (2 * 8));
    GPIOA->MODER |= (0xA << (2 * 8)); // Alternate function
    GPIOA->AFR[1] &= ~(0xFF);
    GPIOA->AFR[1] |= (1 << (4 * 0)) | (1 << (4 * 1));

    // PB6 = IN1 (Left Dir), PB7 = IN2 (Left Dir), PB8 = IN3 (Right Dir), PB9 = IN4 (Right Dir)
    GPIOB->MODER &= ~(0xFF << (2 * 6));
    GPIOB->MODER |= (0x55 << (2 * 6)); // Output mode
    GPIOB->OTYPER &= ~(0xF << 6);

    // Set all INx low initially
    GPIOB->ODR &= ~(0xF << 6);

    TIM1->PSC = 79;
    TIM1->ARR = 999;
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCMR1 |= (6 << 4) | (6 << 12);
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void driveMotor(int side, int speed)
{
    int reverse = 0;
    if (speed < 0)
    {
        reverse = 1;
        speed = -speed;
    }
    if (speed > 1000)
        speed = 1000;

    if (side == MOTOR_LEFT)
    {
        // PB6 = IN1, PB7 = IN2
        if (reverse)
        {
            GPIOB->ODR &= ~(1 << 6); // IN1 = 0
            GPIOB->ODR |= (1 << 7);  // IN2 = 1
        }
        else
        {
            GPIOB->ODR |= (1 << 6);  // IN1 = 1
            GPIOB->ODR &= ~(1 << 7); // IN2 = 0
        }
        TIM1->CCR1 = speed;
    }
    else if (side == MOTOR_RIGHT)
    {
        // PB8 = IN3, PB9 = IN4
        if (reverse)
        {
            GPIOB->ODR &= ~(1 << 8); // IN3 = 0
            GPIOB->ODR |= (1 << 9);  // IN4 = 1
        }
        else
        {
            GPIOB->ODR |= (1 << 8);  // IN3 = 1
            GPIOB->ODR &= ~(1 << 9); // IN4 = 0
        }
        TIM1->CCR2 = speed;
    }
}

void forward(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, speed);
}

void backward(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

void turnLeft(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, speed);
}

void turnRight(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

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
    forward(0);
}
