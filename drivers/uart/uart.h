#ifndef UART_H
#define UART_H

#include "stm32f103xb.h" /* CMSIS device header from common/CMSIS_library */
#include "gpio.h"

#define UART_INSTANCE_MAX 3

/* default baud if none supplied */
#define BAUD_RATE       115200UL

typedef enum {
    UART_OK,
    UART_BUSY,
    UART_TIMEOUT,
    UART_ERROR
} uart_status_t;

typedef enum {
    UART_MODE_BLOCKING,
    UART_MODE_INTERRUPT,
    UART_MODE_DMA
} uart_mode_t;

typedef struct {
    uint32_t baudrate;
    uint8_t parity;
    uint8_t stop_bits;
    uint8_t word_length;
} uart_config_t;

typedef enum
{
    UART_INSTANCE_1 = 0,
    UART_INSTANCE_2,
    UART_INSTANCE_3
} uart_instance_t;

typedef enum
{
  UART_STATE_IDLE = 0,
  UART_STATE_TX_BUSY,
  UART_STATE_RX_BUSY
}uart_state_t;

#define UART_RX_BUFFER_SIZE 256

typedef struct {
    uint8_t buffer[UART_RX_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} uart_rx_buffer_t;

static const USART_TypeDef* uart_instance_map[] __attribute__((unused)) = {
    [UART_INSTANCE_1] = USART1,
    [UART_INSTANCE_2] = USART2,
    [UART_INSTANCE_3] = USART3
};

typedef void (*uart_callback_t)(void);

void uart_init(uart_instance_t instance, uart_config_t *config);
uart_status_t uart_transmit(uart_instance_t instance, uint8_t *data, uint16_t size, uart_mode_t mode);
uart_status_t uart_receive(uart_instance_t instance, uint8_t *buffer, uint16_t size, uart_mode_t mode);
uart_status_t uart_get_state(uart_instance_t instance, uart_state_t *state);
uint16_t uart_get_rx_count(uart_instance_t instance);
void uart_register_callback(uart_instance_t instance, uart_callback_t callback);


#endif /* UART_H */