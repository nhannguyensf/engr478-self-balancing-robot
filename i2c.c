// i2c.c - Basic I2C1 driver for STM32L476RG
#include "i2c.h"
#include "stm32l476xx.h"
#include "led.h"

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

    // STEP 1: Write register address
    I2C1->CR2 = (addr & 0xFE) | (1 << 16); // write, 1 byte
    I2C1->CR2 |= I2C_CR2_START;

    int timeout = 10000;
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

    // STEP 2: Read data
    I2C1->CR2 = (addr & 0xFE) | (1 << 16) | I2C_CR2_RD_WRN;
    I2C1->CR2 |= I2C_CR2_START;

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_RXNE) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;

    data = I2C1->RXDR; // ✅ This is required

    timeout = 10000;
    while (!(I2C1->ISR & I2C_ISR_TC) && --timeout)
        ;
    if (timeout == 0)
        goto i2c_error;

    I2C1->CR2 |= I2C_CR2_STOP;

    return data;

i2c_error:
    while (1)
    {
        toggleLED(); // Blink LED to indicate I2C fail
        volatile int d;
        for (d = 0; d < 100000; d++)
            ;
    }
}

void I2C_ReadBurst(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t len)
{
    // Write the register address
    I2C1->CR2 = (addr & 0xFE) | (1 << 16); // 1 byte to send
    I2C1->CR2 |= I2C_CR2_START;

    while (!(I2C1->ISR & I2C_ISR_TXIS))
        ;
    I2C1->TXDR = reg;

    while (!(I2C1->ISR & I2C_ISR_TC))
        ;

    // Configure for reading 'len' bytes
    I2C1->CR2 = (addr | 0x01) | (len << 16); // Read mode
    I2C1->CR2 |= I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_AUTOEND;

    uint8_t i;
    for (i = 0; i < len; i++)
    {
        while (!(I2C1->ISR & I2C_ISR_RXNE))
            ;
        data[i] = I2C1->RXDR;
    }

    while (!(I2C1->ISR & I2C_ISR_STOPF))
        ;
    I2C1->ICR |= I2C_ICR_STOPCF;
}

void test_I2C_MPU6050(void)
{
    initI2C1();                       // Ensure I2C is initialized
    initLED();                        // Optional: visual indicator
    I2C_Write(0x68 << 1, 0x6B, 0x00); // PWR_MGMT_1 = 0 (wakes up MPU6050)

    uint8_t whoami = I2C_Read(0x68 << 1, 0x75); // WHO_AM_I register

    if (whoami == 0x68)
    {
        // Sensor is responding — turn on LED
        onLED();
    }
    else
    {
        // Sensor not responding — blink LED continuously
        while (1)
        {
            toggleLED();
            volatile int i;
            for (i = 0; i < 100000; i++)
                ; // crude delay
        }
    }
}
