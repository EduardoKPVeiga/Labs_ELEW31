// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"
 // QPNMLKJHGFEDCBA
 // EDCBA9876543210
#define GPIO_PORTA  (0x0001) //bit 0
#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTH	(0x0080) //bit 7 0000 0000 1000 0000
#define GPIO_PORTN  (0x1000) //bit 12

#define CLOCKWISE 													0
#define COUNTERCLOCKWISE										1

void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t valor);
void MotorUnipolarPasso(uint32_t num_passos);
void MotorUnipolarPasso1Volta(void);
void MotorUnipolarMeioPasso(uint32_t num_passos);
void Timer0A_Handler(void);
void GPIOPortJ_Handler(void);

extern uint8_t directionMotor;			//	Sentido de rotação do motor.
extern uint8_t led_status;					//	Estado dos leds da interrupcao

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	// Contar ate 8.000.000
	// 16 bits, prescale de 8 bits
	// GPTMTAILR = 7.999.999
	
	// Timer 0
	SYSCTL_RCGCTIMER_R = 0x1;
	
	// Espera o timer estar pronto
	while ((SYSCTL_PRTIMER_R & 0x01) != 0x01);
	
	// 0xFF.FF.FE.FE, desabilita o timer A
	TIMER0_CTL_R &= 0xFFFFFFFE;
	
	// Timer no modo 32 bits
	TIMER0_CFG_R = 0x0;
	
	// Modo periódico
	TIMER0_TAMR_R = 0x2;
	
	// Timeout de 7.999.999 clocks
	TIMER0_TAILR_R = 0x7A11FF;
	
	// Usado apenas para o prescale
	TIMER0_TAPR_R = 0x00;
	
	// Configuração da interrupção
	TIMER0_ICR_R = 0x1;
	
	// Interrupção no timeout
	TIMER0_IMR_R = 0x00000001;
	
	// Prioridade da interrupção
	NVIC_PRI4_R = (uint32_t)4 << (uint32_t)29;
	
	// Habilita a interrupção do timer
	NVIC_EN0_R = (uint32_t)1 << (uint32_t)19;
	
	// Habilita o timer A
	TIMER0_CTL_R |= 0x01;
	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH) ){}
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTA_AHB_DIR_R = 0x00;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTH_AHB_DIR_R = 0x0F;	// H3 ~ H0
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0x03;   	//Bit0 = entrada e bit1 = saida
	GPIO_PORTJ_AHB_DEN_R = 0x03;		//Bit0 e bit1
	GPIO_PORTH_AHB_DEN_R = 0x0F;		// PH3 ~ PH0
	GPIO_PORTN_DEN_R = 0x03; 		   	//Bit0 e bit1
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   	//Bit0 e bit1
	
	// Função alternativa dos pinos A0 e A1
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	
	// Uart run mode clock gating control
	SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;			// ativa o bit 00000001
	while (SYSCTL_PRUART_R != SYSCTL_PRUART_R0)
		SysTick_Wait1ms(100);
	
	// Uart control
	// RXE(habilita o receptor) = bit9 / TXE(habilita o transmissor) = bit8 / UARTEN(habilita a UART) = bit0
	UART0_CTL_R = 0x00;												// desativando a UART
	
	// Uart Baud Rate
	// Baud Rate 19200 bits/seg
	// BRD = 80M / (16 * BaudRate) = 260,4167
	UART0_IBRD_R = 260;											 	// 16 bits para parte inteira 
	UART0_FBRD_R = 27;												// 6 bits para parte fracionada 0,4167 * 64 = 26,67 = 27
	
	// Num. bits, paridade, stop bits
	/*
	 * bit 6 e bit5 = WLEN: define o tamanho da palavra sendo 5 + bit6bit5(5 a 8 bits)
	 * bit 4 = FEN: habilita a fila
	 * bit 3 = bits de stop
	 * 		0 => 1 bit de stop
	 * 		1 => 2 bits de stop
	 * bit 2
	 *		0 => paridade ímpar
	 *		1 => paridade par
	 * bit 1 = PEN(Parity Enable) : habilita a paridade
   *		0 => desabilita
	 *		1 => habilita
	 * bit 0 = BRK
	 *		0 => normal uso
	 *		1 => nao sei
	 */
	UART0_LCRH_R = 0x78;		// 0111 1000
	// Start(1) + Word(8 bits) + Paridade(0) + Stop(2) = 11 bits
	
	// Select clock source
	UART0_CC_R = 0x00;
	
	// Uart control
	UART0_CTL_R = UART0_CTL_R | 0x301; // setando os bits 0011 0000 0001 habilitando TX, RX, UARTEN(uart enable)
	
	GPIO_PORTA_AHB_PCTL_R = 0x11;			// 0001 0001 A1 e A0 recebe a função 1
	GPIO_PORTA_AHB_AFSEL_R = 0x03;		// 0000 0011 habilita os pinos A1 e A0 para função alternativas
	
	// Desabilita as interrupções na porta J
	GPIO_PORTJ_AHB_IM_R = 0x00;
	
	// Interrupção por borda
	GPIO_PORTJ_AHB_IS_R = 0x00;
	
	// Borda única em ambos os pinos
	GPIO_PORTJ_AHB_IBE_R = 0x00;
	
	// Borda de descida para J0 e de subida para J1
	GPIO_PORTJ_AHB_IEV_R = 0x02;
	
	// Realiza o ACK no registrador para ambos os pinos
	GPIO_PORTJ_AHB_ICR_R = 0x03;
	
	// Ativa a interrupção em ambos os pinos
	GPIO_PORTJ_AHB_IM_R = 0x03
	
	// Ativa a fonte de interrupção no NVIC
	NVIC_EN1_R = (NVIC_EN1_R | (0x01 << 19));
	
	NVIC_PRI12_R = 0x05 << 27;
	
	
	
}

// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortA_Output(uint32_t valor)
{
    uint32_t temp;
    temp = GPIO_PORTA_AHB_DATA_R & 0xF0; // ZERA 4 ULTIMOS BITS
    temp = temp | valor;
    GPIO_PORTA_AHB_DATA_R = temp; 
}

void PortQ_Output(uint32_t valor)
{
    uint32_t temp;
    temp = GPIO_PORTQ_DATA_R & 0x0F; // ZERA 4 PRIMEIROS BITS
    temp = temp | valor;
    GPIO_PORTQ_DATA_R = temp; 
}

void PortH_Output(uint8_t value) {
	uint8_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0xF0; // ZERA 4 ULTIMOS BITS
	temp = temp | value;
	GPIO_PORTH_AHB_DATA_R = temp;	
}

void Timer0A_Handler()
{
	// Limpar o flag de interrupção
	TIMER0_ICR_R = 0x01;
	
	switch (led_status)
	{
		case 0:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0x80);				// PA7
				PortQ_Output(0x00);
			}
			else
			{
				PortA_Output(0x00);
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x01);						// PQ0
			}
			break;
		case 1:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xC0);				// PA7, PA6
				PortQ_Output(0x00);
			}
			else
			{
				PortA_Output(0x00);
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x03);						// PQ0, PQ1
			}
			break;
		case 2:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xE0);				// PA7, PA6, PA5
				PortQ_Output(0x00);
			}
			else
			{
				PortA_Output(0x00);
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x07);						// PQ0, PQ1, PQ2
			}
			break;
		case 3:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xF0);				// PA7, PA6, PA5, PA4
				PortQ_Output(0x00);
			}
			else
			{
				PortA_Output(0x00);
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0F);						// PQ0, PQ1, PQ2, PQ3
			}
			break;
		case 4:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xF0);				// PA7, PA6, PA5, PA4
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x08);						// PQ3
			}
			else
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0x10);				// PA4
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0F);						// PQ0, PQ1, PQ2, PQ3
			}
			break;
		case 5:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xF0);				// PA7, PA6, PA5, PA4
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0C);						// PQ3, PQ2
			}
			else
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0x30);				// PA4, PA5
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0F);						// PQ0, PQ1, PQ2, PQ3
			}
			break;
		case 6:
			if (directionMotor == CLOCKWISE)
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xF0);				// PA7, PA6, PA5, PA4
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0E);						// PQ3, PQ2, PQ1
			}
			else
			{
				PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0x70);				// PA4, PA5, PA6
				PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0F);						// PQ0, PQ1, PQ2, PQ3
			}
			break;
		case 7:
			PortA_Output(GPIO_PORTA_AHB_DATA_R ^ 0xF0);					// PA7, PA6, PA5, PA4
			PortQ_Output(GPIO_PORTQ_DATA_R ^ 0x0F);							// PQ3, PQ2, PQ1, PQ0
			break;
		default:
			break;
	}
}


void GPIOPortJ_Handler()
{
	if ((GPIO_PORTJ_AHB_RIS_R & 0x01) == 0x01)
	{
		// J0 pressionado
	}
	else if ((GPIO_PORTJ_AHB_RIS_R & 0x02) == 0x02)
	{
		// J1 pressionado
	}
}