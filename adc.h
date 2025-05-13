#ifndef __STM32L476G_DISCOVERY_ADC_H
#define __STM32L476G_DISCOVERY_ADC_H

#include "stm32l476xx.h"
#include <stdint.h>

#define ADC_SAMPLE_SIZE 100

// Initialize the ADC peripheral and GPIO pin
void ADC_Init(void);
void ADC_Wakeup(void);
void ADC_Pin_Init(void);
void ADC_Common_Configuration(void);

// Perform one single ADC conversion on channel-5 and return the raw 12-bit result (0–4095)
uint16_t ADC_Read(void);

/// Map a raw ADC reading (0–4095) into [min_val .. max_val]
static inline float ADC_ReadFloat(float min_val, float max_val)
{
    uint16_t raw = ADC_Read();
    return min_val + ((max_val - min_val) * raw) / 4095.0f;
}

#endif /* __STM32L476G_DISCOVERY_ADC_H */
