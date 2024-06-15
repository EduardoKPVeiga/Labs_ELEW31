// motor.h
// Desenvolvido para a placa EK-TM4C1294XL conectado com a placa auxiliar TIVA para controlar um motor
// de passos.
// Este arquivo Header tem as funcoes para controlar um Motor de Passos Unipolar seguindo as conecoes
// esquematizada na placa auxiliar TIVA.
// Uma atualizacao a ser feita e fazer funcoes para motor de passos bipolar ou criar um header especifico.
// para isso.
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 30/05/2024
// Versao: 1.0
/*------------------------------------------------------------------------------------------------------*/
#ifndef __INCLUDED_MOTOR_H__
#define __INCLUDED_MOTOR_H__
/*------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include "tm4c1294ncpdt.h"
/*------------------------------------------------------------------------------------------------------*/
#define NUMBER_OF_MOTOR_STATES							8
#define CLOCKWISE 													0
#define COUNTERCLOCKWISE										1
#define OFF																	0
#define ON																	1

// ======================================
extern volatile uint8_t duty_cycle;
extern volatile uint8_t motor_state;
extern volatile uint8_t motor_rot;
// ======================================

#endif // end of file MOTOR_H
