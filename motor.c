// motor.c - PWM motor control with direction using TIM2 and L298N
#include "motor.h"
#include "stm32l476xx.h"

// Initialize GPIOs and TIM2 for motor PWM and direction control
void initMotors(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN; // TIM2 for PWM

    // PB10 = PWM Motor1 (TIM2_CH3), PA1 = PWM Motor2 (TIM2_CH2)
    GPIOB->MODER &= ~(0x3 << (2 * 10));
    GPIOB->MODER |= (0x2 << (2 * 10)); // Alternate function mode
    GPIOB->AFR[1] &= ~(0xF << (4 * 2));
    GPIOB->AFR[1] |= (1 << (4 * 2)); // AF1 for TIM2_CH3

    GPIOA->MODER &= ~(0x3 << (2 * 1));
    GPIOA->MODER |= (0x2 << (2 * 1)); // Alternate function mode
    GPIOA->AFR[0] &= ~(0xF << (4 * 1));
    GPIOA->AFR[0] |= (1 << (4 * 1)); // AF1 for TIM2_CH2

    // Direction pins
    // PB4 = IN1, PB5 = IN2 (Motor 1)
    // PA4 = IN3, PB0 = IN4 (Motor 2)
    GPIOB->MODER &= ~(0xF << (2 * 4));
    GPIOB->MODER |= (0x5 << (2 * 4));

    GPIOA->MODER &= ~(0x3 << (2 * 4));
    GPIOA->MODER |= (0x1 << (2 * 4));

    GPIOB->MODER &= ~(0x3);
    GPIOB->MODER |= (0x1);

    GPIOB->OTYPER &= ~(0x31);

    GPIOB->ODR &= ~(0x31);
    GPIOA->ODR &= ~(1 << 4);

    // Set up TIM2
    TIM2->PSC = 79;
    TIM2->ARR = 999;
    TIM2->CCR2 = 0;
    TIM2->CCR3 = 0;
    TIM2->CCMR1 |= (6 << 12); // PWM mode 1 on CH2
    TIM2->CCMR2 |= (6 << 4);  // PWM mode 1 on CH3
    TIM2->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E;
    TIM2->CR1 |= TIM_CR1_CEN;
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
        // PB4 = IN1, PB5 = IN2
        if (reverse)
        {
            GPIOB->ODR &= ~(1 << 4);
            GPIOB->ODR |= (1 << 5);
        }
        else
        {
            GPIOB->ODR |= (1 << 4);
            GPIOB->ODR &= ~(1 << 5);
        }
        TIM2->CCR3 = speed; // Motor 1 PWM on PB10
    }
    else if (side == MOTOR_RIGHT)
    {
        // PA4 = IN3, PB0 = IN4
        if (reverse)
        {
            GPIOA->ODR &= ~(1 << 4);
            GPIOB->ODR |= (1 << 0);
        }
        else
        {
            GPIOA->ODR |= (1 << 4);
            GPIOB->ODR &= ~(1 << 0);
        }
        TIM2->CCR2 = speed; // Motor 2 PWM on PA1
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

void testMotor1Forward(void)
{
    GPIOB->ODR |= (1 << 4);  // IN1 = PB4 = 1
    GPIOB->ODR &= ~(1 << 5); // IN2 = PB5 = 0
    TIM2->CCR3 = 1000;       // Full speed on PB10
    volatile int i;
    for (i = 0; i < 1200000; i++)
        ; // ~3 sec delay
    TIM2->CCR3 = 0;
    GPIOB->ODR &= ~((1 << 4) | (1 << 5)); // Stop
}

void testMotor2Forward(void)
{
    GPIOA->ODR |= (1 << 4);  // IN3 = PA4 = 1
    GPIOB->ODR &= ~(1 << 0); // IN4 = PB0 = 0
    TIM2->CCR2 = 1000;       // Full speed on PA1
    volatile int i;
    for (i = 0; i < 1200000; i++)
        ; // ~3 sec delay
    TIM2->CCR2 = 0;
    GPIOA->ODR &= ~(1 << 4);
    GPIOB->ODR &= ~(1 << 0); // Stop
}