// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

//#include <stdint.h>
#include "motor.h"
#include "gpio.h"
#include <string.h>

#define LCD		0
#define POT		1

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void PortA_Output(uint32_t valor);
void PortQ_Output(uint32_t valor);
uint32_t adc_convertion(void);

// Referência das funções do arquivo lcd.s
void LCD_Init(void);
void LCD_Write_Character(char caracter);
void LCD_Write_String(char* string);
void LCD_Move_Cursor(uint8_t position);
void LCD_Reset(void);
void LCD_Command(uint32_t comamnd);
// Referência das funções do arquivo decode.s
uint32_t Read_KeyboardC(void);
// funções do arquivo main
void LCD_WriteLines(char* line1, char* line2);
void LCD_SecondLine(char* line);
char convertNumberCharacter(uint8_t value);
void printVelocity(uint8_t velocity);

int main(void)
{
	// variaveis de controle
	duty_cycle = 0;
	uint32_t buffer_teclado = 0;
	char buffer = 0;
	uint32_t controle = 1;
	//Funcoes de inicializacao 
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	duty_cycle = 0; 
 	LCD_Init();
	// Menu inicial para escolha de como irá controlar o motor
	LCD_Write_String("MOTOR PARADO");
	LCD_Move_Cursor(0xC0);
	LCD_Write_String("LCD - 0|POT - 1");
	while(buffer_teclado != '0' && buffer_teclado != '1') {
		buffer_teclado = Read_KeyboardC();
		buffer = (uint8_t)buffer_teclado;
		buffer_teclado = buffer;
		LCD_Write_Character(buffer); 
		LCD_Move_Cursor(0xCF);
	}
	if(buffer_teclado == '0') {
			LCD_WriteLines("Escolheu Controle","Tela de LCD");
			controle = LCD;
	} else {
		LCD_WriteLines("Escolheu Controle","Potenciometro");
		controle = POT;
	}
	SysTick_Wait1ms(2500);
	if(controle == LCD) {
		buffer = 0;
		LCD_WriteLines("Sentido HOR - 0","Sentido ANT - 1");
		while(buffer != '0' && buffer != '1') {
			buffer_teclado = Read_KeyboardC();
			buffer = (uint8_t)buffer_teclado;
		}
	}
	//*
	// loop principal
	while(1) {
		uint32_t adc_result = adc_convertion();
		LCD_SecondLine("Velocidade: ");
		printVelocity(duty_cycle);
		if (controle == POT)
		{
			if (adc_result < 2047) //COUNTERCLOCKWISE
			{
				motor_rot = COUNTERCLOCKWISE;
				adc_result = 2047 - adc_result;
				duty_cycle = (uint8_t)(((adc_result * 100) / 4095) * 2);
			}
			else //CLOCKWISE
			{
				motor_rot = CLOCKWISE;
				adc_result -= 2048;
				duty_cycle = (uint8_t)(((adc_result * 100) / 4095) * 2);
			}
		}
		else
		{
			buffer_teclado = Read_KeyboardC();
			buffer = (uint8_t)buffer_teclado;
			if(buffer == '0') {
				motor_rot = CLOCKWISE;
				LCD_Move_Cursor(0x88);
				LCD_Write_String("HOR   ");
			} else if(buffer == '1'){
				motor_rot = COUNTERCLOCKWISE;
				LCD_Move_Cursor(0x88);
				LCD_Write_String("ANT   ");
				buffer = 0;
			}
			duty_cycle = (uint8_t)((adc_result * 100) / 4095);
		}
		
		SysTick_Wait1ms(10);
	}
	//*/
}

void LCD_WriteLines(char* line1, char* line2) {
	LCD_Reset();
	LCD_Write_String(line1);
	LCD_Move_Cursor(0xC0);
	LCD_Write_String(line2);
}

void LCD_SecondLine(char* line) {
	uint8_t size_line = (uint8_t)strlen(line); 
	LCD_Move_Cursor(0xC0); 
	LCD_Write_String(line);
	while(size_line <= 16) {
		LCD_Write_Character(' '); 
		size_line++;
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

void printVelocity(uint8_t velocity) {
	LCD_Move_Cursor(0xCC);
	uint8_t algarism = velocity/100;
	LCD_Write_Character(convertNumberCharacter(algarism));
	velocity = velocity - algarism*100;
	
	algarism = velocity/10;
	LCD_Write_Character(convertNumberCharacter(algarism));
	velocity = velocity - algarism*10;
	
	algarism = velocity;
	LCD_Write_Character(convertNumberCharacter(algarism));
	LCD_Write_Character('%');
}

