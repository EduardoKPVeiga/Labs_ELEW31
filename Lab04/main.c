// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

//#include <stdint.h>
#include "motor.h"
#include <string.h>

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortA_Output(uint32_t valor);
void PortQ_Output(uint32_t valor);
uint32_t adc_convertion(void);

int main(void)
{
	//Funcoes de inicializacao 
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	
	// loop principal
	while(1) {
		uint32_t adc_result = adc_convertion();
		duty_cycle = (uint8_t)((adc_result * 100) / 4095);
		SysTick_Wait1ms(100);
	}
}
