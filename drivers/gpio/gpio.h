#ifndef GPIO_H
#define GPIO_H
#include <stdbool.h>

#include "stm32f103xb.h" /* CMSIS device header from common/CMSIS_library */

typedef enum
{
  GPIO_A=0,
  GPIO_B,
  GPIO_C,
  GPIO_D,
  GPIO_E
}gpio_port;

typedef enum
{
    GPIO_OK = 0,
    GPIO_ERROR,
    GPIO_INVALID_PARAM
} gpio_status_t;

typedef enum
{
    INPUT_ANALOG = 0x0,
    INPUT_FLOATING = 0x1,
    INPUT_PULL_UP_DOWN = 0x2,
    OUTPUT_PUSH_PULL = 0x0,
    OUTPUT_OPEN_DRAIN = 0x1,
    OUTPUT_AF_PUSH_PULL = 0x2,
    OUTPUT_AF_OPEN_DRAIN = 0x3
} gpio_cnf_t;


typedef enum
{
    INPUT_MODE = 0x0,
    OUTPUT_MODE_10MHZ = 0x1,
    OUTPUT_MODE_2MHZ = 0x2,
    OUTPUT_MODE_50MHZ = 0x3
} gpio_mode_t;

typedef struct
{
    gpio_port port;
    uint8_t pin;
    gpio_mode_t mode;
    gpio_cnf_t cnf;
} gpio_t;

static const GPIO_TypeDef* gpio_addr_map[] __attribute__((unused)) = {
  [GPIO_A] = GPIOA,
  [GPIO_B] = GPIOB,
  [GPIO_C] = GPIOC,
  [GPIO_D] = GPIOD,
  [GPIO_E] = GPIOE
};

/*function prototypes*/
/*high-level api's used in application code*/
void gpio_setup(gpio_t* gpio);
bool read_gpio(gpio_t* gpio);
void write_gpio(gpio_t* gpio ,bool state);
void toggle_gpio(gpio_t* gpio);

#endif /* GPIO_H */