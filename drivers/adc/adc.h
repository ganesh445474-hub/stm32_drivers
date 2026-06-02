/* adc.h - ADC driver header with include guards */
#ifndef ADC_ADC_H
#define ADC_ADC_H

#include "gpio.h"
#include "stm32f103xb.h" /* CMSIS device header from common/CMSIS_library */

void adc1_SC_init(uint8_t channel);
uint16_t adc1_read(void);
void adc1_start_conversion(void);
void adc1_dma_init(uint16_t* adc_buffer , uint8_t channel);

#endif /* ADC_ADC_H */

