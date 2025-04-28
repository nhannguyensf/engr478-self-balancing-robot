// i2c.c - Basic I2C1 driver for STM32L476RG
#include "i2c.h"
#include "stm32l476xx.h"

void initI2C1(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // Enable GPIOB clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; // Enable I2C1 clock

    // Configure PB8 (SCL) and PB9 (SDA) as AF4 (I2C1)
    GPIOB->MODER &= ~(0xF << (2 * 8));
    GPIOB->MODER |= (0xA << (2 * 8));                 // AF mode
    GPIOB->OTYPER |= (3 << 8);                        // Open-drain
    GPIOB->AFR[1] |= (4 << (4 * 0)) | (4 << (4 * 1)); // AF4 for PB8 and PB9

    // Reset I2C1
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Configure Timing: assuming 4MHz clock, set ~100kHz I2C
    I2C1->TIMINGR = 0x00303D5B; // Safe timing for 4 MHz system clock

    I2C1->CR1 |= I2C_CR1_PE; // Enable I2C1
}

void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
    I2C1->CR2 = (addr & 0xFE) | (1 << 16); // 1 byte
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = reg;

    while (!(I2C1->ISR & I2C_ISR_TC))
        ;

    I2C1->CR2 = (addr & 0xFE) | (1 << 16);
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = data;

    while (!(I2C1->ISR & I2C_ISR_TC))
        ;
    I2C1->CR2 |= I2C_CR2_STOP;
}

uint8_t I2C_Read(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    I2C1->CR2 = (addr & 0xFE) | (1 << 16); // 1 byte to send
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = reg;

    while (!(I2C1->ISR & I2C_ISR_TC))
        ;

    I2C1->CR2 = (addr | 0x1) | (1 << 16); // 1 byte to read
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_RXNE))
        ;
    data = I2C1->RXDR;

    while (!(I2C1->ISR & I2C_ISR_TC))
        ;
    I2C1->CR2 |= I2C_CR2_STOP;

    return data;
}
