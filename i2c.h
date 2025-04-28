// i2c.h - Header for basic I2C driver
#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void initI2C1(void);
void I2C_Write(uint8_t addr, uint8_t reg, uint8_t data);
uint8_t I2C_Read(uint8_t addr, uint8_t reg);

#endif
