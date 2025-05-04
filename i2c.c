// i2c.c - Basic I2C1 driver for STM32L476RG (Improved for MPU6050 compatibility)
#include "i2c.h"
#include "stm32l476xx.h"
#include "led.h"

void initI2C1(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    GPIOB->MODER &= ~(0xF << (2 * 8));
    GPIOB->MODER |= (0xA << (2 * 8));
    GPIOB->OTYPER |= (3 << 8);
    GPIOB->AFR[1] |= (4 << (4 * 0)) | (4 << (4 * 1));

    I2C1->CR1 &= ~I2C_CR1_PE;
    I2C1->TIMINGR = 0x00303D5B;
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data)
{
    int timeout;

    // Write register address and value in a single transaction
    I2C1->CR2 = (addr & 0xFE);
    I2C1->CR2 |= (2 << 16); // NBYTES = 2
    I2C1->CR2 |= I2C_CR2_START | I2C_CR2_AUTOEND;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->TXDR = reg;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->TXDR = data;

    timeout = 10000;
    while ((I2C1->ISR & I2C_ISR_STOPF) == 0 && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->ICR |= I2C_ICR_STOPCF;

    return;

i2c_error:
    while (1)
    {
        toggleLED();
        volatile int d;
        for (d = 0; d < 100000; d++)
            ;
    }
}

uint8_t I2C_Read(uint8_t addr, uint8_t reg)
{
    uint8_t data;
    int timeout;

    // Write register address
    I2C1->CR2 = (addr & 0xFE);
    I2C1->CR2 |= (1 << 16); // NBYTES = 1
    I2C1->CR2 |= I2C_CR2_START;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->TXDR = reg;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TC) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;

    I2C1->CR2 |= I2C_CR2_STOP;
    timeout = 10000;
    while ((I2C1->ISR & I2C_ISR_STOPF) == 0 && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->ICR |= I2C_ICR_STOPCF;

    // Read 1 byte
    I2C1->CR2 = (addr | 0x01);
    I2C1->CR2 |= (1 << 16); // NBYTES = 1
    I2C1->CR2 |= I2C_CR2_START | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_RXNE) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;

    data = I2C1->RXDR;
    return data;

i2c_error:
    while (1)
    {
        toggleLED();
        volatile int d;
        for (d = 0; d < 100000; d++)
            ;
    }
}

void I2C_ReadBurst(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    int timeout;

    // Write register address first
    I2C1->CR2 = (addr & 0xFE);
    I2C1->CR2 |= (1 << 16); // NBYTES = 1
    I2C1->CR2 |= I2C_CR2_START;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->TXDR = reg;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TC) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;

    I2C1->CR2 |= I2C_CR2_STOP;
    timeout = 10000;
    while ((I2C1->ISR & I2C_ISR_STOPF) == 0 && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;
    I2C1->ICR |= I2C_ICR_STOPCF;

    // Read burst
    I2C1->CR2 = (addr | 0x01);
    I2C1->CR2 |= (len << 16);
    I2C1->CR2 |= I2C_CR2_START | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;

    uint8_t i;
    for (i = 0; i < len; i++)
    {
        timeout = 10000;
        while (!(I2C1->ISR & I2C_ISR_RXNE) && --timeout)
            ;
        if (timeout == 0)
            goto i2c_error;
        data[i] = I2C1->RXDR;
    }
    return;

i2c_error:
    while (1)
    {
        toggleLED();
        volatile int d;
        for (d = 0; d < 100000; d++)
            ;
    }
}
