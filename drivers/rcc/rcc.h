#ifndef COMMON_DRIVERS_RCC_INC_RCC_H
#define COMMON_DRIVERS_RCC_INC_RCC_H

/* Include device-specific register definitions. */
#include "stm32f103xb.h"  /* or appropriate device header */
#include "system_stm32f1xx.h"

/* RCC driver header */

/* Initialize clock system (HSI as system clock) */
void rcc_init(void);

#endif /* COMMON_DRIVERS_RCC_INC_RCC_H */
