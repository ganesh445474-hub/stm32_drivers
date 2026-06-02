#include "adc.h"

void adc1_SC_init(uint8_t channel){

    /* 2. GPIO Setup for PA7 (LDR Sensor) as Analog Input */
    gpio_t ldr_pin = {
        .port = GPIO_A,
        .pin = channel,
        .mode = INPUT_MODE,
        .cnf = INPUT_ANALOG
    };
    gpio_setup(&ldr_pin);

/* 3. Manual ADC1 Configuration */
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
RCC->CFGR &= ~RCC_CFGR_ADCPRE;
RCC->CFGR |= (3U << 14); // Prescaler /8

/* 1. Wake up ADC from power down */
ADC1->CR2 |= ADC_CR2_ADON; 
delay_ms(1); // Small delay to ensure stability

/* 2. Run Calibration */
ADC1->CR2 |= ADC_CR2_CAL;
while (ADC1->CR2 & ADC_CR2_CAL);

/* 3. Configure for Software Trigger */
// Set EXTSEL to 0b111 (Software Start) and enable EXTTRIG
ADC1->CR2 |= (7U << 17) | ADC_CR2_EXTTRIG;

/* 4. Configure Sample Time */
ADC1->SMPR2 &= ~(0x7 << (3*channel)); // Clear bits for Channel 7 (3*7 = 21)
ADC1->SMPR2 |= (0x7 << (3*channel));  // Set 239.5 cycles for Ch 7

}

void adc1_start_conversion(void) {
    /* Select channel 7 for conversion in the first sequence slot */
        ADC1->SQR3 = 7;
        ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t adc1_read(void) {
   /* Wait for conversion to complete */
        while (!(ADC1->SR & ADC_SR_EOC));

        /* Read the converted value */
        uint16_t ret_val = ADC1->DR;
        return ret_val;
}


void adc1_dma_init(uint16_t* adc_buffer , uint8_t channel) {

    /* 2. GPIO Setup for PA7 (LDR Sensor) as Analog Input */
    gpio_t ldr_pin = {
        .port = GPIO_A,
        .pin = channel,
        .mode = INPUT_MODE,
        .cnf = INPUT_ANALOG
    };
    gpio_setup(&ldr_pin);

    /* 1. Enable Clocks */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN;
    RCC->AHBENR  |= RCC_AHBENR_DMA1EN;

    /* 2. DMA1 Channel 1 Setup (Hardwired to ADC1) */
    DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));   // Source: ADC Data Register
    DMA1_Channel1->CMAR = (uint32_t)(adc_buffer);   // Destination: Variable
    DMA1_Channel1->CNDTR = 1;                        // 1 Transfer (only PA7)
    
    // CCR Config: 
    // MSIZE/PSIZE = 01 (16-bit), MINC = 0 (No increment), CIRC = 1, DIR = 0
    DMA1_Channel1->CCR = (0x1 << 10) | (0x1 << 8) | DMA_CCR_CIRC | DMA_CCR_EN;

    /* 3. ADC Setup */
    RCC->CFGR |= (3U << 14); // Prescaler /8
    
    // Wake up ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    delay_ms(1);
    
    // Calibrate
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    // Continuous Mode + DMA Enable + SWSTART Trigger
    ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA | (7U << 17) | ADC_CR2_EXTTRIG;
    
   /* 4. Configure Sample Time */
    ADC1->SMPR2 &= ~(0x7 << (3*channel)); // Clear bits for Channel 7 (3*7 = 21)
    ADC1->SMPR2 |= (0x7 << (3*channel));  // Set 239.5 cycles for Ch 7 
    
    // Channel Sequence (PA7 is IN7)
    ADC1->SQR3 = 7;

    // Start the continuous cycle
    ADC1->CR2 |= ADC_CR2_SWSTART;
}