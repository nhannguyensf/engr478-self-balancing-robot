// motor.c - PWM motor control with direction using TIM3 and L298N
#include "motor.h"
#include "stm32l476xx.h"

// Initialize GPIOs and TIM3 for motor PWM and direction control
void initMotors(void)
{
    // Enable clocks for GPIOA, GPIOB, and TIM3
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN; // TIM3 for PWM on PA6/PA7

    // PA6 = PWM Left (TIM3_CH1), PA7 = PWM Right (TIM3_CH2)
    GPIOA->MODER &= ~(0xF << (2 * 6)); // Clear mode bits
    GPIOA->MODER |= (0xA << (2 * 6));  // Set AF mode
    GPIOA->AFR[0] &= ~(0xFF << (4 * 6));
    GPIOA->AFR[0] |= (2 << (4 * 6)) | (2 << (4 * 7)); // AF2 for TIM3

    // PB6/7 = Left motor direction (IN1/IN2), PB4/5 = Right motor direction (IN3/IN4)
    GPIOB->MODER &= ~(0xF << (2 * 4));
    GPIOB->MODER |= (0x5 << (2 * 4)); // PB4, PB5 = output
    GPIOB->MODER &= ~(0xF << (2 * 6));
    GPIOB->MODER |= (0x5 << (2 * 6)); // PB6, PB7 = output
    GPIOB->OTYPER &= ~(0xF << 4);     // Push-pull

    GPIOB->ODR &= ~(0xF << 4); // Set all direction pins low initially

    // Configure TIM3 for PWM
    TIM3->PSC = 79;  // Prescaler for 50 kHz timer clock (4 MHz / 80)
    TIM3->ARR = 999; // Auto-reload for 1 kHz PWM
    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCMR1 |= (6 << 4) | (6 << 12);         // PWM mode 1 for CH1 and CH2
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // Enable PWM outputs
    TIM3->CR1 |= TIM_CR1_CEN;                    // Start TIM3
}

// Drive individual motor with signed speed value
void driveMotor(int side, int speed)
{
    int reverse = 0;
    if (speed < 0)
    {
        reverse = 1;
        speed = -speed;
    }
    if (speed > 1000)
        speed = 1000; // Limit to max PWM

    if (side == MOTOR_LEFT)
    {
        // PB6 = IN1, PB7 = IN2
        if (reverse)
        {
            GPIOB->ODR &= ~(1 << 6);
            GPIOB->ODR |= (1 << 7);
        }
        else
        {
            GPIOB->ODR |= (1 << 6);
            GPIOB->ODR &= ~(1 << 7);
        }
        TIM3->CCR1 = speed; // Left motor PWM
    }
    else if (side == MOTOR_RIGHT)
    {
        // PB4 = IN3, PB5 = IN4
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
        TIM3->CCR2 = speed; // Right motor PWM
    }
}

// Drive both motors forward
void forward(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, speed);
}

// Drive both motors backward
void backward(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

// Turn left in place
void turnLeft(int speed)
{
    driveMotor(MOTOR_LEFT, -speed);
    driveMotor(MOTOR_RIGHT, speed);
}

// Turn right in place
void turnRight(int speed)
{
    driveMotor(MOTOR_LEFT, speed);
    driveMotor(MOTOR_RIGHT, -speed);
}

// Run a test sequence: forward, left, right, backward, stop
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
    forward(0); // Stop
}
