#ifndef __GPIO_H_
#define __GPIO_H_

#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "motor.h"

 // QPNMLKJHGFEDCBA
 // EDCBA9876543210
#define GPIO_PORTA  (0x0001) //bit 0
#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTQ	(0x4000) //bit 14
#define GPIO_PORTH	(0x0080) //bit 7 0000 0000 1000 0000
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTP	(0x2000) //bit 13
#define GPIO_PORTF	(0x0020) //bit 5
#define GPIO_PORTE	(0x0010) //bit 4
#define GPIO_PORTK	(0x0200) //bit 9	0000 0010 0000 0000
#define GPIO_PORTM	(0x0800) //bit 11 0000 1000 0000 0000 
#define GPIO_PORTL	(0x0400) //bit 10 0000 0100 0000 0000

void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void Timer0A_init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t valor);
void Timer0A_Handler(void);
void GPIOPortJ_Handler(void);
void PortJ_interrupt_init(void);
void ADC0_init(void);
void PortH_Output(uint8_t value);
void PortA_Output(uint32_t valor);
void PortQ_Output(uint32_t valor);
void PortE_Output(uint8_t value);
uint32_t PortE_Input(void);
uint32_t adc_convertion(void);

#endif
