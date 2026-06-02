#include "rcc.h"

void rcc_init(void)
{
    /* 1. Configure Flash wait states for 72 MHz */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* Optional but recommended: enable prefetch buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* 2. Enable HSE */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    /* 3. Configure prescalers */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   // AHB = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  // APB1 = HCLK/2 = 36 MHz
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;  // APB2 = HCLK = 72 MHz

    /* 4. Configure PLL: HSE as source, x9 multiplier */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC;      // HSE as PLL source
    RCC->CFGR |= RCC_CFGR_PLLMULL9;    // 8 MHz * 9 = 72 MHz

    /* 5. Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* 6. Switch system clock to PLL */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

     SysTick_Config(72000); // Configure SysTick for 1 ms interrupts
}
