// led.c - Basic LED control for STM32L476RG Nucleo
#include "led.h"
#include "stm32l476xx.h"

void initLED(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIOA clock

    // Configure PA5 (LD2 on Nucleo) as output
    GPIOA->MODER &= ~(3UL << (5 * 2));
    GPIOA->MODER |= (1UL << (5 * 2)); // Output mode
}

void toggleLED(void)
{
    GPIOA->ODR ^= (1UL << 5); // Toggle PA5
}

void onLED(void)
{
    GPIOA->ODR |= (1UL << 5); // Set PA5 high
}

void offLED(void)
{
    GPIOA->ODR &= ~(1UL << 5); // Set PA5 low
}
