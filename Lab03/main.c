// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void Pisca_leds(void);
void WriteTX(char value);
uint8_t ReadRX(void);
void acender_ledsN(uint8_t vn);
void clean_putty(void);
void move_cursor_beginning_putty(void);
void Write_line_putty(char* line);

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	uint8_t var = 0x00;
	while (1)
	{
		Write_line_putty("Digite um comando:");
		var = ReadRX();
		clean_putty();
		move_cursor_beginning_putty();
		//SysTick_Wait1ms(1000);
		switch(var) {
			case '0':
				acender_ledsN(0x00);
				break;
			case 	'1':
				acender_ledsN(0x01); // acende uma led
				break;
			case	't':
				acender_ledsN(0x03);	// acende todas as leds
				break;
			default:
				break;
		}
		//acender_ledsN(0x03);
		//SysTick_Wait1ms(3000);
    //WriteTX(var);
	}
}

void acender_ledsN(uint8_t vn) {
	PortN_Output(vn);
}

void Pisca_leds(void)
{
	PortN_Output(0x2);
	SysTick_Wait1ms(250);
	PortN_Output(0x1);
	SysTick_Wait1ms(250);
}
