#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "drivers/rcc/rcc.h"
#include "drivers/gpio/gpio.h"
#include "drivers/uart/uart.h"
#include "adc.h"
#include "stm32f103xb.h" /* CMSIS device header from common/CMSIS_library */

volatile uint32_t ms_ticks = 0;
volatile uint16_t adc_buffer = 0;

void SysTick_Handler(void)
{
    ms_ticks++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms);
}

int main(void)
{
    /* Initialize clocks and peripheral drivers */
    rcc_init();

    /* Configure SysTick for 1ms intervals (System Clock = 72MHz) */
    SysTick_Config(72000);

    /* 1. UART1 Setup: 115200, 8N1 */
    uart_config_t uart_cfg = {
        .baudrate = 115200,
        .parity = 0,
        .stop_bits = 1,
        .word_length = 8
    };
    uart_init(UART_INSTANCE_1, &uart_cfg);

    
    
    adc1_dma_init(&adc_buffer, 7); // Channel 7 for PA7 (LDR)
    uint16_t ldr_value = 0;

    while (1)
    {
        /* Delay for 10ms as requested */
        delay_ms(10);

        /* Read the converted value */
        ldr_value = adc_buffer;

        /* Format and print the value over UART */
        char mesg[50];
        char prefix[] = "sensor Value: ";
        int i = 0, j = 0;

        /* Copy prefix */
        while (prefix[j] != '\0') {
            mesg[i++] = prefix[j++];
        }

        /* Convert integer to string manually */
        uint16_t temp = ldr_value;
        char digits[6];
        int k = 0;

        if (temp == 0) {
            digits[k++] = '0';
        } else {
            while (temp > 0) {
                digits[k++] = (temp % 10) + '0';
                temp /= 10;
            }
        }

        while (k > 0) {
            mesg[i++] = digits[--k];
        }

        mesg[i++] = '\r';
        mesg[i++] = '\n';
        mesg[i] = '\0';

        /* Calculate length */
        int len = 0;
        while (mesg[len] != '\0') len++;

        uart_transmit(UART_INSTANCE_1, (uint8_t*)mesg, (uint16_t)len, UART_MODE_BLOCKING);
    }

    return 0;
}
