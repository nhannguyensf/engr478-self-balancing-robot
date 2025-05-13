#ifndef __STM32L476G_DISCOVERY_ADC_H
#define __STM32L476G_DISCOVERY_ADC_H

#include "stm32l476xx.h"

#define ADC_SAMPLE_SIZE 100

void ADC_Init(void);

void ADC_Wakeup(void);
void ADC_Init(void);

void ADC_Pin_Init(void);
void ADC_Common_Configuration(void);

#endif /* __STM32L476G_DISCOVERY_ADC_H */
