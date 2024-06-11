// motor.h
// Desenvolvido para a placa EK-TM4C1294XL conectado com a placa auxiliar TIVA para controlar um motor
// de passos.
// Este arquivo Header tem as funçoes para controlar um Motor de Passos Unipolar seguindo as coneções
// esquematizada na placa auxiliar TIVA.
// Uma atualização a ser feita é fazer funções para motor de passos bipolar ou criar um header específico.
// para isso.
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 30/05/2024
// Versão: 1.0
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
/*------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------*/
#endif // end of file MOTOR_H
/*------------------------------------------------------------------------------------------------------*/