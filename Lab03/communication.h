// communication.h
// Desenvolvido para a placa EK-TM4C1294XL.
// Este arquivo Header tem as funçoes para comunicar com um computador fazendo a recepção e transmissão
// de dados utilizando a interface Uart(interface serial).
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 30/05/2024
// Versão: 1.0
/*------------------------------------------------------------------------------------------------------*/
#ifndef __INCLUDED_COMMUNICATION_H__
#define __INCLUDED_COMMUNICATION_H__
/*------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "tm4c1294ncpdt.h"
/*------------------------------------------------------------------------------------------------------*/
// define
#define BAUD_RATE																				19200		// bits/s
#define WORD_LENGTH																					8		// comprimento da palavra comunicada [5,8]
#define STOP_BITS																						1		// bits [1,2]
#define PARITY_ENABLE																				1		// habilitar a paridade
#define ODD																									0		// ímpar
#define EVEN																								1		// par
#define PARITY																						ODD		// paridade ímpar
/*------------------------------------------------------------------------------------------------------*/
/*
 * Função de inicialização da interface Uart.
 * detalhe: GPIO_RCGCUART deve ser habilitado o clock para port A no GPIO_Init.
 * Entrada:	Não tém
 * Saída:		Não tém
 */
void Uart_init(void);
uint8_t ReadRX(void);
uint8_t ReadWord(char** word);
void WriteTX(char value);
void WriteTXNumber(char value);
void WriteWord(char* word);
void clean_putty(void);
void move_cursor_beginning_putty(void);
void Write_line_putty(char* line);
/*------------------------------------------------------------------------------------------------------*/
#endif	// end of file
/*------------------------------------------------------------------------------------------------------*/