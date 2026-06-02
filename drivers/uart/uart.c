#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* USART1 on APB2 = 72MHz, USART2/3 on APB1 = 36MHz */
#define SYSCLK_HZ_APB2   72000000UL
#define SYSCLK_HZ_APB1   36000000UL

/* BRR value = fPCLK / baud */
#define USART1_BRR_VAL(baud) ((SYSCLK_HZ_APB2 + (baud/2)) / baud)
#define USART2_BRR_VAL(baud) ((SYSCLK_HZ_APB1 + (baud/2)) / baud)
#define USART3_BRR_VAL(baud) ((SYSCLK_HZ_APB1 + (baud/2)) / baud)

/* driver state */
static uart_state_t uart_states[UART_INSTANCE_MAX] = { UART_STATE_IDLE };
static uart_callback_t uart_callbacks[UART_INSTANCE_MAX] = { 0 };
static uart_rx_buffer_t uart_rx_buffers[UART_INSTANCE_MAX] = { 0 };

/* internal helpers */
static USART_TypeDef *get_uart(uart_instance_t instance)
{
    if (instance >= UART_INSTANCE_MAX) {
        return NULL;
    }
    return (USART_TypeDef *)uart_instance_map[instance];
}

static void enable_uart_clock(uart_instance_t instance)
{
    switch (instance) {
    case UART_INSTANCE_1:
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        /* GPIOA clock for pins */
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        break;
    case UART_INSTANCE_2:
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        /* GPIOA clock for pins */
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        break;
    case UART_INSTANCE_3:
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        /* GPIOB clock for pins */
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        break;
    default:
        /* instance not supported */
        break;
    }
}

/* configure pins used by UART instances */
static void configure_uart_pins(uart_instance_t instance)
{
    gpio_t gp;

    switch (instance) {
    case UART_INSTANCE_1:
        /* PA9 = TX (AF push-pull), PA10 = RX (floating input) */
        gp.port = GPIO_A;
        gp.pin = 9;
        gp.mode = OUTPUT_MODE_50MHZ;
        gp.cnf  = OUTPUT_AF_PUSH_PULL;
        gpio_setup(&gp);

        gp.pin = 10;
        gp.mode = INPUT_MODE;
        gp.cnf  = INPUT_FLOATING;
        gpio_setup(&gp);
        break;

    case UART_INSTANCE_2:
        /* PA2 = TX (AF push-pull), PA3 = RX (floating input) */
        gp.port = GPIO_A;
        gp.pin = 2;
        gp.mode = OUTPUT_MODE_50MHZ;
        gp.cnf  = OUTPUT_AF_PUSH_PULL;
        gpio_setup(&gp);

        gp.pin = 3;
        gp.mode = INPUT_MODE;
        gp.cnf  = INPUT_FLOATING;
        gpio_setup(&gp);
        break;

    case UART_INSTANCE_3:
        /* PB10 = TX (AF push-pull), PB11 = RX (floating input) */
        gp.port = GPIO_B;
        gp.pin = 10;
        gp.mode = OUTPUT_MODE_50MHZ;
        gp.cnf  = OUTPUT_AF_PUSH_PULL;
        gpio_setup(&gp);

        gp.pin = 11;
        gp.mode = INPUT_MODE;
        gp.cnf  = INPUT_FLOATING;
        gpio_setup(&gp);
        break;

    default:
        break;
    }
}


void uart_init(uart_instance_t instance, uart_config_t *config)
{
    USART_TypeDef *uart = get_uart(instance);
    if (uart == NULL || config == NULL) {
        return;
    }

    enable_uart_clock(instance);
    configure_uart_pins(instance);

    /* disable until configuration complete */
    uart->CR1 &= ~USART_CR1_UE;

    /* baud rate - different for APB1 vs APB2 */
    uint32_t baud = (config->baudrate == 0) ? BAUD_RATE : config->baudrate;
    uint32_t usartdiv;
    if (instance == UART_INSTANCE_1) {
        usartdiv = USART1_BRR_VAL(baud);
    } else {
        usartdiv = USART2_BRR_VAL(baud);
    }
    uint32_t mantissa = usartdiv / 16;
    uint32_t fraction = usartdiv % 16;
    uart->BRR = (mantissa << 4) | fraction;

    /* word length/ parity/ stop bits omitted for simplicity */
    uart->CR2 &= ~(3U<<12); /* 1 stop bit */
    uart->CR1 &= ~USART_CR1_PCE; /* no parity */
    uart->CR1 &= ~USART_CR1_M; /* 8 data bits */
    /* enable transmitter and receiver */
    uart->CR1 |= USART_CR1_TE | USART_CR1_RE;

    /* enable RX interrupt */
    uart->CR1 |= USART_CR1_RXNEIE;

    /* enable NVIC interrupt */
    switch (instance) {
    case UART_INSTANCE_1:
        NVIC_EnableIRQ(USART1_IRQn);
        break;
    case UART_INSTANCE_2:
        NVIC_EnableIRQ(USART2_IRQn);
        break;
    case UART_INSTANCE_3:
        NVIC_EnableIRQ(USART3_IRQn);
        break;
    default:
        break;
    }

    /* finally, enable UART */
    uart->CR1 |= USART_CR1_UE;

    /* initialize RX buffer */
    uart_rx_buffers[instance].head = 0;
    uart_rx_buffers[instance].tail = 0;
    uart_rx_buffers[instance].count = 0;

    uart_states[instance] = UART_STATE_IDLE;
}

uart_status_t uart_transmit(uart_instance_t instance, uint8_t *data, uint16_t size, uart_mode_t mode)
{
    USART_TypeDef *uart = get_uart(instance);
    if (uart == NULL || data == NULL || size == 0) {
        return UART_ERROR;
    }

    if (mode != UART_MODE_BLOCKING) {
        return UART_ERROR; /* only blocking supported */
    }

    uart_states[instance] = UART_STATE_TX_BUSY;

    for (uint16_t i = 0; i < size; ++i) {
        /* wait until TXE (transmit data register empty) */
        while ((uart->SR & USART_SR_TXE) == 0) {
            ;
        }
        uart->DR = data[i];
    }

    uart_states[instance] = UART_STATE_IDLE;
    if (uart_callbacks[instance]) {
        uart_callbacks[instance]();
    }

    return UART_OK;
}

uart_status_t uart_receive(uart_instance_t instance, uint8_t *buffer, uint16_t size, uart_mode_t mode)
{
    USART_TypeDef *uart = get_uart(instance);
    if (uart == NULL || buffer == NULL || size == 0) {
        return UART_ERROR;
    }

    if (mode == UART_MODE_BLOCKING) {
        uart_states[instance] = UART_STATE_RX_BUSY;

        for (uint16_t i = 0; i < size; ++i) {
            /* wait until data received */
            while ((uart->SR & USART_SR_RXNE) == 0) {
                ;
            }
            buffer[i] = (uint8_t)(uart->DR & 0xFF);
        }

        uart_states[instance] = UART_STATE_IDLE;
        if (uart_callbacks[instance]) {
            uart_callbacks[instance]();
        }

        return UART_OK;
    }
    else if (mode == UART_MODE_INTERRUPT) {
        uint16_t bytes_read = 0;
        uart_rx_buffer_t *rx_buf = &uart_rx_buffers[instance];

        /* disable interrupts temporarily */
        __disable_irq();

        while (bytes_read < size && rx_buf->count > 0) {
            buffer[bytes_read++] = rx_buf->buffer[rx_buf->tail];
            rx_buf->tail = (rx_buf->tail + 1) % UART_RX_BUFFER_SIZE;
            rx_buf->count--;
        }

        __enable_irq();

        if (uart_callbacks[instance]) {
            uart_callbacks[instance]();
        }

        return (bytes_read == size) ? UART_OK : UART_BUSY; /* UART_BUSY if not enough data */
    }

    return UART_ERROR; /* unsupported mode */
}

uart_status_t uart_get_state(uart_instance_t instance, uart_state_t *state)
{
    if (state == NULL || instance >= UART_INSTANCE_MAX) {
        return UART_ERROR;
    }

    *state = uart_states[instance];
    return UART_OK;
}

uint16_t uart_get_rx_count(uart_instance_t instance)
{
    if (instance >= UART_INSTANCE_MAX) {
        return 0;
    }

    return uart_rx_buffers[instance].count;
}

void uart_register_callback(uart_instance_t instance, uart_callback_t callback)
{
    if (instance >= UART_INSTANCE_MAX) {
        return;
    }
    uart_callbacks[instance] = callback;
}

/* ISR helper function */
static void uart_rx_isr_handler(uart_instance_t instance)
{
    USART_TypeDef *uart = get_uart(instance);
    if (uart == NULL) {
        return;
    }

    /* check if RXNE flag is set */
    if (uart->SR & USART_SR_RXNE) {
        uint8_t data = (uint8_t)(uart->DR & 0xFF);
        uart_rx_buffer_t *rx_buf = &uart_rx_buffers[instance];

        /* store data in circular buffer if there's space */
        if (rx_buf->count < UART_RX_BUFFER_SIZE) {
            rx_buf->buffer[rx_buf->head] = data;
            rx_buf->head = (rx_buf->head + 1) % UART_RX_BUFFER_SIZE;
            rx_buf->count++;
        }
        /* if buffer is full, data is lost (could add error handling here) */
    }
}

/* USART ISRs */
void USART1_IRQHandler(void)
{
    uart_rx_isr_handler(UART_INSTANCE_1);
}

void USART2_IRQHandler(void)
{
    uart_rx_isr_handler(UART_INSTANCE_2);
}

void USART3_IRQHandler(void)
{
    uart_rx_isr_handler(UART_INSTANCE_3);
}

