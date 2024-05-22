// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"
  
#define GPIO_PORTA  (0x0001) //bit 1
#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTN  (0x1000) //bit 12

void SysTick_Wait1ms(uint32_t delay);

// -------------------------------------------------------------------------------
// Fun��o GPIO_Init
// Inicializa os ports J e N
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA);
	//1b.   ap�s isso verificar no PRGPIO se a porta est� pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA) ){};
	
	// 2. Limpar o AMSEL para desabilitar a anal�gica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for sa�da
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem fun��o alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0x02;   //Bit0 e bit1
	GPIO_PORTJ_AHB_DEN_R = 0x03;   //Bit0 e bit1
	GPIO_PORTN_DEN_R = 0x03; 		   //Bit0 e bit1
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   //Bit0 e bit1
	
	// Fun��o alternativa dos pinos A0 e A1
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	
	// Uart run mode clock gating control
	SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;
	while (SYSCTL_PRUART_R != SYSCTL_PRUART_R0)
		SysTick_Wait1ms(100);
	
	// Uart control
	UART0_CTL_R = 0x00;
	
	// Uart Baud Rate
	UART0_IBRD_R = 260;
	UART0_FBRD_R = 27;
	
	// Num. bits, paridade, stop bits
	UART0_LCRH_R = 0x78;
	
	// Select clock source
	UART0_CC_R = 0x00;
	
	// Uart control
	UART0_CTL_R = UART0_CTL_R | 0x301;
}

// -------------------------------------------------------------------------------
// Fun��o PortJ_Input
// L� os valores de entrada do port J
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Fun��o PortN_Output
// Escreve os valores no port N
// Par�metro de entrada: Valor a ser escrito
// Par�metro de sa�da: n�o tem
void PortN_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amig�vel nos bits 0 e 1
    temp = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na fun��o
    temp = temp | valor;
    GPIO_PORTN_DATA_R = temp; 
}

uint8_t ReadRX()
{
	while ((UART0_FR_R && 0x10) == 0x10)
		SysTick_Wait1ms(1);
	uint8_t value = UART0_DR_R;
	return value;
}

void WriteTX(uint8_t value)
{
	while ((UART0_FR_R && 0x20) == 0x20)
		SysTick_Wait1ms(1);
	UART0_DR_R = value;
}


