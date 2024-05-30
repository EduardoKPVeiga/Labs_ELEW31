// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

//#include <stdint.h>
#include "motor.h"
#include "communication.h"

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
int main(void)
{
	// variáveis de controle
	uint8_t directionMotor;									//	Sentido de rotação do motor.
	uint8_t	velocity;												// 	Motor é meio passo ou passo completo.
	stepState	state = step_state_0;					// 	Máquina de estado para controlar o passo do motor de passo.
	float	angle = 0.00f;										//	Ângulo do motor de passos.
	uint8_t	revolutions;										// 	n.° de voltas do motor.
	char word[50], ch;
	uint8_t size;
	
	//Funções de inicialização 
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Uart_init();
	// loop principal
	while(1) {
		ch = ReadRX();
		// to tentando ler uma string em vez de um caracter.
		while(ch != 0x1D) {
			WriteTX(ch);
			WriteTXNumber(ch);
			ch = ReadRX();
		}
		WriteTX('*');
	}
}
