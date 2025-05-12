/* button.h */
#ifndef BUTTON_H
#define BUTTON_H

#include "stm32l476xx.h"

// Structure for Button configuration
typedef struct
{
    GPIO_TypeDef *port; // GPIO port (e.g., GPIOC)
    uint32_t pin;       // GPIO pin number (e.g., 13)
    uint32_t exti_line; // EXTI line (e.g., 13)
    IRQn_Type irq_num;  // Corresponding IRQ number (e.g., EXTI15_10_IRQn)
} Button_t;

// Debounce delay
void debounce_Delay(uint32_t delay_ms);

// Configure a push-button to generate external interrupts
void configure_Button_interrupt(Button_t button);

// Flag: 0 = balancing stopped, 1 = balancing running
extern volatile uint8_t program_running;

#endif // BUTTON_H