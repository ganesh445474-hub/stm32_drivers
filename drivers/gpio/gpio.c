#include "gpio.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* helpers */
static inline void enable_gpio_clock(gpio_port port)
{
    /* RCC APB2ENR bits for GPIOA..GPIOE are contiguous starting at bit 2 */
    if (port <= GPIO_E) {
        RCC->APB2ENR |= (1U << (RCC_APB2ENR_IOPAEN_Pos + (uint32_t)port));
    }
}

/* public API implementations */
void gpio_setup(gpio_t *gpio)
{
    if (gpio == NULL) {
        return;
    }

    /* validate port */
    if (gpio->port > GPIO_E) {
        return;
    }

    /* validate pin */
    if (gpio->pin > 15) {
        return;
    }

    /* enable peripheral clock */
    enable_gpio_clock(gpio->port);

    GPIO_TypeDef *port_base = (GPIO_TypeDef *)gpio_addr_map[gpio->port];
    if (port_base == NULL) {
        return;
    }

    volatile uint32_t *cfg_reg;
    uint32_t shift;

    /* select CRL or CRH */
    if (gpio->pin < 8) {
        cfg_reg = &port_base->CRL;
        shift = gpio->pin * 4U;
    } 
    else {
        cfg_reg = &port_base->CRH;
        shift = (gpio->pin - 8U) * 4U;
    }

    /* build configuration value */
    uint32_t config = ((uint32_t)gpio->mode & 0x3U) |
                      ((uint32_t)gpio->cnf  << 2U);

    /* clear existing 4-bit field */
    *cfg_reg &= ~(0xFU << shift);

    /* write new configuration */
    *cfg_reg |= (config << shift);

    
}

bool read_gpio(gpio_t *gpio)
{
    if (gpio == NULL || gpio->pin > 15) {
        return false;
    }

    GPIO_TypeDef *port_base = (GPIO_TypeDef *)gpio_addr_map[gpio->port];
    if (port_base == NULL) {
        return false;
    }

    return ((port_base->IDR & (1U << gpio->pin)) != 0U);
}

void write_gpio(gpio_t *gpio, bool state)
{
    if (gpio == NULL || gpio->pin > 15) {
        return;
    }

    GPIO_TypeDef *port_base = (GPIO_TypeDef *)gpio_addr_map[gpio->port];
    if (port_base == NULL) {
        return;
    }

    if (state) {
        port_base->BSRR = (1U << gpio->pin);
    } else {
        port_base->BRR = (1U << gpio->pin);
    }
}

void toggle_gpio(gpio_t *gpio)
{
    if (gpio == NULL || gpio->pin > 15) {
        return;
    }

    GPIO_TypeDef *port_base = (GPIO_TypeDef *)gpio_addr_map[gpio->port];
    if (port_base == NULL) {
        return;
    }

    port_base->ODR ^= (1U << gpio->pin);
}


