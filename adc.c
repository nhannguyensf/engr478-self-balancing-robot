#include "ADC.h"
#include "LED.h"
#include "stm32l476xx.h"
#include <stdint.h>

// Analog Inputs:
//    PA1 (ADC12_IN6), PA2 (ADC12_IN7)
//    These pins are not used: PA0 (ADC12_IN5, PA3 (ADC12_IN8)

//-------------------------------------------------------------------------------------------
// STM32L4x6xx Errata sheet
// When the delay between two consecutive ADC conversions is higher than 1 ms the result of
// the second conversion might be incorrect. The same issue occurs when the delay between the
// calibration and the first conversion is higher than 1 ms.
// Workaround
// When the delay between two ADC conversions is higher than the above limit, perform two ADC
// consecutive conversions in single, scan or continuous mode: the first is a dummy conversion
// of any ADC channel. This conversion should not be taken into account by the application.

//-------------------------------------------------------------------------------------------
// ADC Wakeup
// By default, the ADC is in deep-power-down mode where its supply is internally switched off
// to reduce the leakage currents.
//-------------------------------------------------------------------------------------------
void ADC_Wakeup(void)
{

    int wait_time;

    // To start ADC operations, the following sequence should be applied
    // DEEPPWD = 0: ADC not in deep-power down
    // DEEPPWD = 1: ADC in deep-power-down (default reset state)
    if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
        ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state

    // Enable the ADC internal voltage regulator
    // Before performing any operation such as launching a calibration or enabling the ADC, the ADC
    // voltage regulator must first be enabled and the software must wait for the regulator start-up time.
    ADC1->CR |= ADC_CR_ADVREGEN;

    // Wait for ADC voltage regulator start-up time
    // The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP)
    // before launching a calibration or enabling the ADC.
    // T_ADCVREG_STUP = 20 us
    wait_time = 20 * (80000000 / 1000000);
    while (wait_time != 0)
    {
        wait_time--;
    }
}

//-------------------------------------------------------------------------------------------
// 	ADC Common Configuration
//-------------------------------------------------------------------------------------------
void ADC_Common_Configuration(void)
{
    // 1) Enable I/O analog-switches voltage booster for low VDDA
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;

    // 2) Enable internal reference voltage (VREFINT)
    ADC123_COMMON->CCR |= ADC_CCR_VREFEN;

    // 3) Select HSI16 (16 MHz) as ADC clock source
    //    ADCSEL = 0b11 → RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1
    RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
    RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;

    // 4) Asynchronous clock mode (CKMODE = 00)
    ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;

    // 5) Prescaler ÷4 for async source (PRESC = 0b0010)
    ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;
    ADC123_COMMON->CCR |= ADC_CCR_PRESC_1;

    // 6) Independent (single-ADC) mode
    ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
}

//-------------------------------------------------------------------------------------------
// 	ADC Pin Initialization
//  PA1 (ADC12_IN6), PA2 (ADC12_IN7)
//-------------------------------------------------------------------------------------------
void ADC_Pin_Init(void)
{
    // Enable the clock of GPIO Port A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // GPIO Pin Initialization
    // GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
    // GPIO Output Type: Output push-pull (0, reset), Output open drain (1)

    // GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
    // Configure PA1 (ADC12_IN6), PA2 (ADC12_IN7) as Analog
    GPIOA->MODER |= 3U << (2 * 0); // PA0 mode = 11 (Analog)

    // GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~(3U << (2 * 0)); // PA0 no pull-up/pull-down

    // GPIO port analog switch control register (ASCR)
    // 0: Disconnect analog switch to the ADC input (reset state)
    // 1: Connect analog switch to the ADC input
    GPIOA->ASCR |= GPIO_ASCR_EN_0; // Connect analog switch for PA0
}

//-------------------------------------------------------------------------------------------
// Initialize ADC
//-------------------------------------------------------------------------------------------
void ADC_Init(void)
{

    // Enable the clock of ADC
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST;
    (void)RCC->AHB2RSTR; // short delay
    RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST;

    ADC_Pin_Init();
    ADC_Common_Configuration();
    ADC_Wakeup();

    // ADC control register 1 (ADC_CR1)
    // L1: ADC1->CR1			&= ~(ADC_CR1_RES);							//
    ADC1->CFGR &= ~ADC_CFGR_RES;   // Resolution, (00 = 12-bit, 01 = 10-bit, 10 = 8-bit, 11 = 6-bit)
    ADC1->CFGR &= ~ADC_CFGR_ALIGN; // Data Alignment (0 = Right alignment, 1 = Left alignment)

    // L1: ADC1->CR1			&= ~(ADC_CR1_SCAN);							// Scan mode disabled

    // ADC regular sequence register 1 (ADC_SQR1)
    // L1: ADC1->SQR1 		&= ~ADC_SQR1_L; 							  // 00000: 1 conversion in the regular channel conversion sequence
    ADC1->SQR1 &= ~ADC_SQR1_L; // 0000: 1 conversion in the regular channel conversion sequence

    // Specify the channel number of the 1st conversion in regular sequence
    // L1: ADC1->SQR5 		|= (5 & ADC_SQR5_SQ1);	// SQ1[4:0] bits (1st conversion in regular sequence)
    ADC1->SQR1 &= ~ADC_SQR1_SQ1;
    ADC1->SQR1 |= (5U << 6);              // channel 5
    ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_5; // single-ended on IN5

    // ADC Sample Time
    // This sampling time must be enough for the input voltage source to charge the embedded
    // capacitor to the input voltage level.
    // Software is allowed to write these bits only when ADSTART=0 and JADSTART=0
    //   000: 2.5 ADC clock cycles      001: 6.5 ADC clock cycles
    //   010: 12.5 ADC clock cycles     011: 24.5 ADC clock cycles
    //   100: 47.5 ADC clock cycles     101: 92.5 ADC clock cycles
    //   110: 247.5 ADC clock cycles    111: 640.5 ADC clock cycles

    // ADC_SMPR3_SMP5 = Channel 5 Sample time selection
    // L1: ADC1->SMPR3 		&= ~ADC_SMPR3_SMP5;		// sample time for first channel, NOTE: These bits must be written only when ADON=0.
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP5; // clear sample-time for IN5
    ADC1->SMPR1 |= 1U << 15;        // e.g. 24.5 cycles @80MHz

    // ADC control register 2 (ADC_CR2)
    // L1: ADC1->CR2 			&=  ~ADC_CR2_CONT;    // Disable Continuous conversion mode
    ADC1->CFGR &= ~ADC_CFGR_CONT; // ADC Single/continuous conversion mode for regular conversion

    // L1: NVIC_SetPriority(ADC1_IRQn, 1); // Set Priority to 1
    // L1: NVIC_EnableIRQ(ADC1_IRQn);      // Enable interrupt form ACD1 peripheral

    // L1: ADC1->CR1 		  |= ADC_CR1_EOCIE; 							// Enable interrupt: End Of Conversion
    // ADC1->IER |= ADC_IER_EOC;  // Enable End of Regular Conversion interrupt
    // ADC1->IER |= ADC_IER_EOS;            // Enable ADC End of Regular Sequence of Conversions Interrupt
    // NVIC_EnableIRQ(ADC1_2_IRQn);

    // Configuring the trigger polarity for regular external triggers
    // 00: Hardware Trigger detection disabled, software trigger detection enabled
    // 01: Hardware Trigger with detection on the rising edge
    // 10: Hardware Trigger with detection on the falling edge
    // 11: Hardware Trigger with detection on both the rising and falling edges
    ADC1->CFGR &= ~ADC_CFGR_EXTEN;

    // Enable ADC1
    // L1: ADC1->CR2  |= ADC_CR2_ADON;     // Turn on conversion
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
        ;

    // L1: ADC1->CR2  |= ADC_CR2_CFG;       // ADC configuration: 0: Bank A selected; 1: Bank B selected
    // L1: ADC1->CR2	|= ADC_CR2_SWSTART;		// Start Conversion of regular channels
    // L1: while(ADC1->CR2 & ADC_CR2_CFG);	// Wait until configuration completes
}

//-------------------------------------------------------------------------------------------
// 	ADC 1/2 Interrupt Handler
//-------------------------------------------------------------------------------------------
void ADC1_2_IRQHandler(void)
{
    NVIC_ClearPendingIRQ(ADC1_2_IRQn);

    // ADC End of Conversion (EOC)
    if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC)
    {
        // It is cleared by software writing 1 to it or by reading the corresponding ADCx_JDRy register
        ADC1->ISR |= ADC_ISR_EOC;
    }

    // ADC End of Injected Sequence of Conversions  (JEOS)
    if ((ADC1->ISR & ADC_ISR_EOS) == ADC_ISR_EOS)
    {
        // It is cleared by software writing 1 to it.
        ADC1->ISR |= ADC_ISR_EOS;
    }
}

/// Single-conversion, blocking read of ADC1 (channel 5)
uint16_t ADC_Read(void)
{
    // 1) Start software conversion
    ADC1->CR |= ADC_CR_ADSTART;

    // 2) Wait until end-of-conversion
    while ((ADC1->ISR & ADC_ISR_EOC) == 0)
    {
    }

    // 3) Read and return 12-bit result
    return (uint16_t)(ADC1->DR & ADC_DR_RDATA);
}
