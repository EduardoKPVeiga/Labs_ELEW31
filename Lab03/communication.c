// communication.c
// Arquivo de implementação do header communication.h
// Este arquivo em C tem as funçoes para comunicação com placa EK-TM4C1294XL utilizando a interface Uart.
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 31/05/2024
// Versão: 1.0

#include "communication.h"
#include <stdlib.h>

void Uart_init(void) {
	// Passo 1: Habilitar o clock no módulo UART
	SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;							// ativa o bit 00000001
	while (SYSCTL_PRUART_R != SYSCTL_PRUART_R0) {}
	// Passo 2: Garantir que a UART esteja desabilitada antes de fazer as alterações (limpar o bit UARTEN)
	// no registrador UARTCTL;
		UART0_CTL_R = 0x00;
	// Passo 3: Escrever o baud-rate nos registradores UARTIBRD e UARTFBRD
	// Uart Baud Rate
	// Baud Rate 19200 bits/seg
	// BRD = 80M / (16 * BaudRate) = 260,4167
	/* double bd 	= 80*1000000/ (16 * BAUD_RATE);
		uint16_t i 	= (uint16_t)bd;
		uint8_t f		= ceil((bd - i) * 64);
	 */
	UART0_IBRD_R 	= 260;											 	// 16 bits para parte inteira 
	UART0_FBRD_R 	= 27;													// 6 bits para parte fracionada 0,4167 * 64 = 26,67 = 27
	// Passo 4: Configurar o registrador UARTLCRH para o número de bits, paridade, stop bits e fila
	uint8_t uart_lcrh	=	0;
	uart_lcrh  = (WORD_LENGTH - 5) << 5; 	// preencho os bits 6 e 5
	uart_lcrh |= (1 << 4);   							// habilitando a fila bit 4
	uart_lcrh |= (STOP_BITS - 1) << 3;		// configutando os bits de stop
	uart_lcrh |= (PARITY) << 2;
	uart_lcrh |= (PARITY_ENABLE) << 1;
	uart_lcrh &= 0xFE;									 	// zerando ultimo bit (0x FE = 1111 1110)
	UART0_LCRH_R = uart_lcrh;
	// Passo 5: Garantir que a fonte de clock seja o clock do sistema no registrador UARTCC escrevendo 0
	UART0_CC_R = 0x00;
	// Passo 6: Habilitar as flags RXE, TXE e UARTEN no registrador UARTCTL
	UART0_CTL_R = UART0_CTL_R | 0x301; // setando os bits 0011 0000 0001 habilitando TX, RX, UARTEN(uart enable)
	// Passo 7: Habilitar o clock no módulo GPIO no registrador RCGGPIO (cada bit representa uma GPIO) e
	// esperar até que a respectiva GPIO esteja pronta para ser acessada no registrador PRGPIO (cada
	// bit representa uma GPIO).
	// feito no GPIO_Init
	// Passo 8: Desabilitar a funcionalidade analógica no registrador GPIOAMSEL.
	GPIO_PORTA_AHB_AMSEL_R 	= 0x00;
	// Passo 9: Escolher a função alternativa dos pinos respectivos TX e RX no registrador GPIOPCTL
	GPIO_PORTA_AHB_PCTL_R 	= 0x11;
	// Passp 10: Habilitar os bits de função alternativa no registrador GPIOAFSEL nos pinos 
	// respectivos à UART.
	GPIO_PORTA_AHB_AFSEL_R = 0x03;		// 0000 0011 habilita os pinos A1 e A0 para função alternativas
	// Passo 11: 11.Configurar os pinos como digitais no registrador GPIODEN.
	GPIO_PORTA_AHB_DEN_R = 0x03;   	//Bit0 = entrada e bit1 = saida
}

uint8_t ReadRX(void) {
	if( (UART0_FR_R & 0x10) == 0x10) {
		return 0x00;
	}
	return (uint8_t)UART0_DR_R;
}

uint8_t ReadRXLoop(void)
{
	// bit 4 => RXFE indica que não é possivel a leitura
	while ((UART0_FR_R & 0x10) == 0x10){}	// 0001 0000 => verifica se é possivel a leitura
		//SysTick_Wait1ms(1);
	uint8_t value = (uint8_t)UART0_DR_R;
	return value;
}

uint8_t ReadWord(char word[][50]) {
	uint8_t size = 0;
	char ch = (char)ReadRXLoop();
	while (ch != '/' && ch != '*') {
		(*word)[size++] = ch;
		ch = (char)ReadRXLoop();
	}
	(*word)[size++] = '\0';
	if(ch == '*') {
		return 0;
	}
	return size;
}

// inicia a transmissão de dados
void WriteTX(char value)
{
	// bit 5 => TXFF indica que não é possivel a escrita
	while ((UART0_FR_R & 0x20) == 0x20){} // 0010 0000 => verifica se é possivel a escrita
		//SysTick_Wait1ms(1);
	UART0_DR_R = (char)value;
}

void WriteTXCodigo(char value) {
	WriteTX(' ');
	WriteTX('(');
	WriteTX('0');
	WriteTX('x');
	char ch;
	switch(value/16) {
		case 1:
			ch = '1';
			break;
		case 2:
			ch = '2';
			break;
		case 3:
			ch = '1';
			break;
		case 4:
			ch = '4';
			break;
		case 5:
			ch = '5';
			break;
		case 6:
			ch = '6';
			break;
		case 7:
			ch = '7';
			break;
		case 8:
			ch = '8';
			break;
		case 9:
			ch = '9';
			break;
		case 10:
			ch = 'A';
			break;
		case 11:
			ch = 'B';
			break;
		case 12:
			ch = 'C';
			break;
		case 13:
			ch = 'D';
			break;
		case 14:
			ch = 'E';
			break;
		case 15:
			ch = 'F';
			break;
	}
	WriteTX(ch);
	switch(value%16) {
		case 1:
			ch = '1';
			break;
		case 2:
			ch = '2';
			break;
		case 3:
			ch = '1';
			break;
		case 4:
			ch = '4';
			break;
		case 5:
			ch = '5';
			break;
		case 6:
			ch = '6';
			break;
		case 7:
			ch = '7';
			break;
		case 8:
			ch = '8';
			break;
		case 9:
			ch = '9';
			break;
		case 10:
			ch = 'A';
			break;
		case 11:
			ch = 'B';
			break;
		case 12:
			ch = 'C';
			break;
		case 13:
			ch = 'D';
			break;
		case 14:
			ch = 'E';
			break;
		case 15:
			ch = 'F';
			break;
	}
	WriteTX(ch);
	WriteTX(')');
	WriteTX(' ');
}

void WriteWord(char* word) {
	uint8_t i = 0;
	while(word[i] != '\0') {
		WriteTX(word[i]);
		i++;
	}
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
void move_cursor_line_position(uint8_t jumps) {
	move_cursor_beginning_putty();
	while(jumps--) {
		WriteTX('\n');
	}
}
char convertNumberCharacter(uint8_t value) {
	char ch;
	switch(value) {
		case 0:
			ch = '0';
			break;
		case 1:
			ch = '1';
			break;
		case 2:
			ch = '2';
			break;
		case 3:
			ch = '3';
			break;
		case 4:
			ch = '4';
			break;
		case 5:
			ch = '5';
			break;
		case 6:
			ch = '6';
			break;
		case 7:
			ch = '7';
			break;
		case 8:
			ch = '8';
			break;
		case 9:
			ch = '9';
			break;
		default:
			ch = '*';
			break;
	}
	return ch;
}

uint8_t convertCharacterNumber(char string[]) {
	uint8_t i=0,value=0;
	while(string[i] != '\0') {
		value = value * 10;
		value += string[i] - '0';
		i++;
	}
	return value;
}

void WriteTXNumber8(uint8_t number) {
	uint8_t div = 100;
	uint8_t current_digit = number/div;
	while(current_digit == 0 && div > 1) {
		div = div/10;
		current_digit=number/div;
	}
	WriteTX(convertNumberCharacter(current_digit));
	number = number - current_digit*div;
	while(div > 1) {
		div = div / 10;
		current_digit=number/div;
		WriteTX(convertNumberCharacter(current_digit));
		number = number - current_digit*div;
	}
}

void WriteTXDouble(double value, uint8_t precision) {
	uint16_t div = 100;
	uint8_t current_digit=value/div,i;
	char ch;
	while(current_digit == 0 && div > 1) {
		div = div/10;
		current_digit=value/div;
	}
	ch = convertNumberCharacter(current_digit);
	if(ch == '*') {
		return;
	}
	WriteTX(ch);
	value = value - current_digit*div;
	while(div > 1) {
		div = div / 10;
		current_digit=value/div;
		ch = convertNumberCharacter(current_digit);
		if(ch == '*') {
			return;
		}
		WriteTX(ch);
		value = value - current_digit*div;
	}
	WriteTX('.');
	i=0;
	while(precision != i) {
		value = value*10;
		current_digit=(uint8_t)value;
		ch = convertNumberCharacter(current_digit);
		if(ch == '*') {
			return;
		}
		WriteTX(ch);
		value = value - current_digit;
		i++;
	}
}
void WriteTXAngle(double angle) {
	WriteTXDouble(angle, 2);
	WriteWord(" graus");
}