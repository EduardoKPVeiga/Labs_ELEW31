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

void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t valor);
void WriteTX(char value);
uint8_t ReadRX(void);
void clean_putty(void);
void move_cursor_beginning_putty(void);
void Write_line_putty(char* line);
void MotorUnipolarPasso(uint32_t num_passos);
void MotorUnipolarPasso1Volta(void);
void MotorUnipolarMeioPasso(uint32_t num_passos);
void Pisca_leds(void);
void Timer0A_Handler(void);

// -------------------------------------------------------------------------------
// Fun��o GPIO_Init
// Inicializa os ports J e N
// Par�metro de entrada: N�o tem
// Par�metro de sa�da: N�o tem
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
	
	// Modo peri�dico
	TIMER0_TAMR_R = 0x2;
	
	// Timeout de 7.999.999 clocks
	TIMER0_TAILR_R = 0x7A11FF;
	
	// Usado apenas para o prescale
	TIMER0_TAPR_R = 0x00;
	
	// Configura��o da interrup��o
	TIMER0_ICR_R = 0x1;
	
	// Interrup��o no timeout
	TIMER0_IMR_R = 0x00000001;
	
	// Prioridade da interrup��o
	NVIC_PRI4_R = (uint32_t)4 << (uint32_t)29;
	
	// Habilita a interrup��o do timer
	NVIC_EN0_R = (uint32_t)1 << (uint32_t)19;
	
	// Habilita o timer A
	TIMER0_CTL_R |= 0x01;
	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH);
	//1b.   ap�s isso verificar no PRGPIO se a porta est� pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH) ){}
	
	// 2. Limpar o AMSEL para desabilitar a anal�gica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for sa�da
	GPIO_PORTA_AHB_DIR_R = 0x00;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTH_AHB_DIR_R = 0x0F;	// H3 ~ H0
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem fun��o alternativa	
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
	
	// Fun��o alternativa dos pinos A0 e A1
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
	 *		0 => paridade �mpar
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
	
	GPIO_PORTA_AHB_PCTL_R = 0x11;			// 0001 0001 A1 e A0 recebe a fun��o 1
	GPIO_PORTA_AHB_AFSEL_R = 0x03;		// 0000 0011 habilita os pinos A1 e A0 para fun��o alternativas
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
    temp = GPIO_PORTN_DATA_R & 0xFC; // 1111 1100
    //agora vamos fazer o OR com o valor recebido na fun��o
    temp = temp | valor;
    GPIO_PORTN_DATA_R = temp; 
}

void PortH_Output(uint8_t value) {
	uint8_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0xF0; // ZERA 4 ULTIMOS BITS
	temp = temp | value;
	GPIO_PORTH_AHB_DATA_R = temp;
void Timer0A_Handler()
{
	// Limpar o flag de interrup��o
	TIMER0_ICR_R = 0x01;
	Pisca_leds();
}

uint8_t ReadRX(void)
{
	// bit 4 => RXFE indica que n�o � possivel a leitura
	while ((UART0_FR_R & 0x10) == 0x10){}	// 0001 0000 => verifica se � possivel a leitura
		//SysTick_Wait1ms(1);
	uint8_t value = (uint8_t)UART0_DR_R;
	return value;
}

// inicia a transmiss�o de dados
void WriteTX(char value)
{
	// bit 5 => TXFF indica que n�o � possivel a escrita
	while ((UART0_FR_R & 0x20) == 0x20){} // 0010 0000 => verifica se � possivel a escrita
		//SysTick_Wait1ms(1);
	UART0_DR_R = (char)value;
}

void clean_putty(void) {
	WriteTX(0x1B);
	WriteTX('[');
	WriteTX('2');
	WriteTX('J');
}

void move_cursor_beginning_putty(void) {
	WriteTX(0x1B);
	WriteTX('[');
	WriteTX(';');
	WriteTX('H');
}

void Write_line_putty(char* line) {
	while(*line != '\0') {
		WriteTX(*line);
		line++;
	}
	//WriteTX('\n');
}

void Pisca_leds()
{
	PortN_Output(GPIO_PORTN_DATA_R ^ 0x03);
	//WriteTX('A');
}
