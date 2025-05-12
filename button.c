/* button.c */
#include "button.h"
#include "led.h"
#include "tim6_balance.h" // TIM6 definitions

volatile uint8_t program_running = 0; // 0 = stopped, 1 = running
const uint32_t debounce_time_ms = 20;

/**
 * Simple software debounce delay approximation.
 * delay_ms: number of milliseconds to delay (approx.).
 */
void debounce_Delay(uint32_t delay_ms)
{
    volatile uint32_t count = delay_ms * 4000U;
    while (count--)
    {
        __NOP(); // prevent optimization
    }
}

/**
 * Configure a GPIO pin to generate an EXTI interrupt on falling edge.
 */
void configure_Button_interrupt(Button_t button)
{
    // 1) Enable GPIO port clock
    if (button.port == GPIOA)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (button.port == GPIOB)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (button.port == GPIOC)
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // 2) Configure pin as input + pull-up
    button.port->MODER &= ~(3U << (button.pin * 2)); // input mode
    button.port->PUPDR &= ~(3U << (button.pin * 2));
    button.port->PUPDR |= (1U << (button.pin * 2)); // pull-up

    // 3) Enable SYSCFG and map EXTI line
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    uint32_t idx = button.exti_line >> 2;
    uint32_t shift = (button.exti_line & 3U) * 4;
    SYSCFG->EXTICR[idx] &= ~(0xFU << shift);
    if (button.port == GPIOC)
    {
        SYSCFG->EXTICR[idx] |= (2U << shift); // port C = 0x2
    }

    // 4) Configure falling edge trigger, unmask line
    EXTI->FTSR1 |= (1U << button.exti_line);
    EXTI->RTSR1 &= ~(1U << button.exti_line);
    EXTI->IMR1 |= (1U << button.exti_line);

    // 5) Clear any pending interrupt and enable NVIC
    EXTI->PR1 |= (1U << button.exti_line);
    NVIC_ClearPendingIRQ(button.irq_num);
    NVIC_SetPriority(button.irq_num, 2);
    NVIC_EnableIRQ(button.irq_num);
}

/**
 * EXTI15_10 IRQ Handler (for PC13 user button)
 */
void EXTI15_10_IRQHandler(void)
{
    // Only handle if line 13 pending
    if (EXTI->PR1 & EXTI_PR1_PIF13)
    {
        // a) Mask further EXTI interrupts
        NVIC_DisableIRQ(EXTI15_10_IRQn);

        // b) Debounce delay
        debounce_Delay(debounce_time_ms);

        // c) Check if button still pressed (PC13 low)
        // Toggle run/stop flag
        program_running = !program_running;

        // Indicate state
        if (program_running)
            onLED();
        else
            offLED();

        // d) Clear pending and re-enable EXTI
        EXTI->PR1 |= EXTI_PR1_PIF13;
        debounce_Delay(debounce_time_ms);
        NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}
