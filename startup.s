    .syntax unified
    .cpu cortex-m3
    .thumb

/* ========================= */
/* Vector Table              */
/* ========================= */
    .section .isr_vector, "a", %progbits
    .word   _estack             /* Initial Stack Pointer */
    .word   Reset_Handler       /* Reset Handler */
    .word   NMI_Handler
    .word   HardFault_Handler
    .word	MemManage_Handler
    .word	BusFault_Handler
    .word	UsageFault_Handler
    .word	0
    .word	0
    .word	0
    .word	0
    .word	SVC_Handler
    .word	DebugMon_Handler
    .word	0
    .word	PendSV_Handler
    .word	SysTick_Handler
    .word	WWDG_IRQHandler           			/* Window Watchdog interrupt                        */
    .word	PVD_IRQHandler            			/* PVD through EXTI line detection interrupt        */
    .word	TAMPER_IRQHandler         			/* Tamper interrupt                                 */
    .word	RTC_IRQHandler            			/* RTC global interrupt                             */
    .word	FLASH_IRQHandler          			/* Flash global interrupt                           */
    .word	RCC_IRQHandler            			/* RCC global interrupt                             */
    .word	EXTI0_IRQHandler          			/* EXTI Line0 interrupt                             */
    .word	EXTI1_IRQHandler          			/* EXTI Line1 interrupt                             */
    .word	EXTI2_IRQHandler          			/* EXTI Line2 interrupt                             */
    .word	EXTI3_IRQHandler          			/* EXTI Line3 interrupt                             */
    .word	EXTI4_IRQHandler          			/* EXTI Line4 interrupt                             */
    .word	DMA1_Channel1_IRQHandler  			/* DMA1 Channel1 global interrupt                   */
    .word	DMA1_Channel2_IRQHandler  			/* DMA1 Channel2 global interrupt                   */
    .word	DMA1_Channel3_IRQHandler  			/* DMA1 Channel3 global interrupt                   */
    .word	DMA1_Channel4_IRQHandler  			/* DMA1 Channel4 global interrupt                   */
    .word	DMA1_Channel5_IRQHandler  			/* DMA1 Channel5 global interrupt                   */
    .word	DMA1_Channel6_IRQHandler  			/* DMA1 Channel6 global interrupt                   */
    .word	DMA1_Channel7_IRQHandler  			/* DMA1 Channel7 global interrupt                   */
    .word	ADC1_2_IRQHandler         			/* ADC1 and ADC2 global interrupt                   */
    .word	USB_HP_CAN_TX_IRQHandler  			/* USB High Priority or CAN TX interrupts           */
    .word	USB_LP_CAN_RX0_IRQHandler 			/* USB Low Priority or CAN RX0 interrupts           */
    .word	CAN_RX1_IRQHandler        			/* CAN RX1 interrupt                                */
    .word	CAN_SCE_IRQHandler        			/* CAN SCE interrupt                                */
    .word	EXTI9_5_IRQHandler        			/* EXTI Line[9:5] interrupts                        */
    .word	TIM1_BRK_IRQHandler       			/* TIM1 Break interrupt                             */
    .word	TIM1_UP_IRQHandler        			/* TIM1 Update interrupt                            */
    .word	TIM1_TRG_COM_IRQHandler   			/* TIM1 Trigger and Commutation interrupts          */
    .word	TIM1_CC_IRQHandler        			/* TIM1 Capture Compare interrupt                   */
    .word	TIM2_IRQHandler           			/* TIM2 global interrupt                            */
    .word	TIM3_IRQHandler           			/* TIM3 global interrupt                            */
    .word	TIM4_IRQHandler           			/* TIM4 global interrupt                            */
    .word	I2C1_EV_IRQHandler        			/* I2C1 event interrupt                             */
    .word	I2C1_ER_IRQHandler        			/* I2C1 error interrupt                             */
    .word	I2C2_EV_IRQHandler        			/* I2C2 event interrupt                             */
    .word	I2C2_ER_IRQHandler        			/* I2C2 error interrupt                             */
    .word	SPI1_IRQHandler           			/* SPI1 global interrupt                            */
    .word	SPI2_IRQHandler           			/* SPI2 global interrupt                            */
    .word	USART1_IRQHandler         			/* USART1 global interrupt                          */
    .word	USART2_IRQHandler         			/* USART2 global interrupt                          */
    .word	USART3_IRQHandler         			/* USART3 global interrupt                          */
    .word	EXTI15_10_IRQHandler      			/* EXTI Line[15:10] interrupts                      */
    .word	RTCAlarm_IRQHandler       			/* RTC Alarms through EXTI line interrupt           */
    .word	0                         			/* Reserved                                         */
    .word	TIM8_BRK_IRQHandler       			/* TIM8 Break interrupt                             */
    .word	TIM8_UP_IRQHandler        			/* TIM8 Update interrupt                            */
    .word	TIM8_TRG_COM_IRQHandler   			/* TIM8 Trigger and Commutation interrupts          */
    .word	TIM8_CC_IRQHandler        			/* TIM8 Capture Compare interrupt                   */
    .word	ADC3_IRQHandler           			/* ADC3 global interrupt                            */
    .word	FSMC_IRQHandler           			/* FSMC global interrupt                            */
    .word	SDIO_IRQHandler           			/* SDIO global interrupt                            */
    .word	TIM5_IRQHandler           			/* TIM5 global interrupt                            */
    .word	SPI3_IRQHandler           			/* SPI3 global interrupt                            */
    .word	UART4_IRQHandler          			/* UART4 global interrupt                           */
    .word	UART5_IRQHandler          			/* UART5 global interrupt                           */
    .word	TIM6_IRQHandler           			/* TIM6 global interrupt                            */
    .word	TIM7_IRQHandler           			/* TIM7 global interrupt                            */
    .word	DMA2_Channel1_IRQHandler  			/* DMA2 Channel1 global interrupt                   */
    .word	DMA2_Channel2_IRQHandler  			/* DMA2 Channel2 global interrupt                   */
    .word	DMA2_Channel3_IRQHandler  			/* DMA2 Channel3 global interrupt                   */
    .word	DMA2_Channel4_5_IRQHandler			/* DMA2 Channel4 and DMA2 Channel5 global interrupt */

/* ========================= */
/* Reset Handler             */
/* ========================= */
    .section .text.Reset_Handler
    .weak   Reset_Handler
    .type   Reset_Handler, %function

Reset_Handler:

    /* Copy .data from Flash to RAM */
    ldr r0, =_sidata     /* Source in Flash */
    ldr r1, =_sdata      /* Destination in RAM */
    ldr r2, =_edata      /* End of .data */

Copy_Data:
    cmp r1, r2
    ittt lt
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt Copy_Data

    /* Zero initialize .bss */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

Zero_BSS:
    cmp r0, r1
    itt lt
    strlt r2, [r0], #4
    blt Zero_BSS

    /* Call main */
   bl main

Infinite_Loop:
    b Infinite_Loop

/* ========================= */
/* Default Handlers          */
/* ========================= */
    .weak NMI_Handler
    .weak HardFault_Handler
    .weak	MemManage_Handler

	.weak	BusFault_Handler

	.weak	UsageFault_Handler

	.weak	SVC_Handler

	.weak	DebugMon_Handler

	.weak	PendSV_Handler

	.weak	SysTick_Handler

	.weak	WWDG_IRQHandler

	.weak	PVD_IRQHandler

	.weak	TAMPER_IRQHandler

	.weak	RTC_IRQHandler

	.weak	FLASH_IRQHandler

	.weak	RCC_IRQHandler

	.weak	EXTI0_IRQHandler

	.weak	EXTI1_IRQHandler

	.weak	EXTI2_IRQHandler

	.weak	EXTI3_IRQHandler

	.weak	EXTI4_IRQHandler

	.weak	DMA1_Channel1_IRQHandler

	.weak	DMA1_Channel2_IRQHandler

	.weak	DMA1_Channel3_IRQHandler

	.weak	DMA1_Channel4_IRQHandler

	.weak	DMA1_Channel5_IRQHandler

	.weak	DMA1_Channel6_IRQHandler

	.weak	DMA1_Channel7_IRQHandler

	.weak	ADC1_2_IRQHandler

	.weak	USB_HP_CAN_TX_IRQHandler

	.weak	USB_LP_CAN_RX0_IRQHandler

	.weak	CAN_RX1_IRQHandler

	.weak	CAN_SCE_IRQHandler

	.weak	EXTI9_5_IRQHandler

	.weak	TIM1_BRK_IRQHandler

	.weak	TIM1_UP_IRQHandler

	.weak	TIM1_TRG_COM_IRQHandler

	.weak	TIM1_CC_IRQHandler

	.weak	TIM2_IRQHandler

	.weak	TIM3_IRQHandler

	.weak	TIM4_IRQHandler

	.weak	I2C1_EV_IRQHandler

	.weak	I2C1_ER_IRQHandler

	.weak	I2C2_EV_IRQHandler

	.weak	I2C2_ER_IRQHandler

	.weak	SPI1_IRQHandler

	.weak	SPI2_IRQHandler

	.weak	USART1_IRQHandler

	.weak	USART2_IRQHandler

	.weak	USART3_IRQHandler

	.weak	EXTI15_10_IRQHandler

	.weak	RTCAlarm_IRQHandler

	.weak	TIM8_BRK_IRQHandler

	.weak	TIM8_UP_IRQHandler

	.weak	TIM8_TRG_COM_IRQHandler

	.weak	TIM8_CC_IRQHandler

	.weak	ADC3_IRQHandler

	.weak	FSMC_IRQHandler

	.weak	SDIO_IRQHandler

	.weak	TIM5_IRQHandler

	.weak	SPI3_IRQHandler

	.weak	UART4_IRQHandler

	.weak	UART5_IRQHandler

	.weak	TIM6_IRQHandler

	.weak	TIM7_IRQHandler

	.weak	DMA2_Channel1_IRQHandler

	.weak	DMA2_Channel2_IRQHandler

	.weak	DMA2_Channel3_IRQHandler

	.weak	DMA2_Channel4_5_IRQHandler

	.weak	SystemInit

NMI_Handler:
HardFault_Handler:
    b .