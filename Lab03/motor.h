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
#define HALFSTEPMODE												0
#define FULLSTEPMODE												1
#define ANGLE_PER_STATE_FULLLSTEP						360.0/2048
#define ANGLE_PER_STATE_HALFSTEP						360.0/4096
#define STATE_TRANSITION_DELAY							7							//	delay de 10ms
/*------------------------------------------------------------------------------------------------------*/
typedef enum stepState {
	step_state_0,
	step_state_0_1,					
	step_state_1,
	step_state_1_2,
	step_state_2,
	step_state_2_3,
	step_state_3,
	step_state_3_0
}stepState;
/*------------------------------------------------------------------------------------------------------*/
/*
 * Função para manuntenção da configuração de estados do motor.
 * Caso o motor esteja num estado de meio passo ele irá para o passo seguinte conforme sua direção de
 * rotação.
 * entrada: stepState(estado do motor de passos unipolar passado como referência);
 * 					direction(direção de rotação do motor de passos unipolar);
 *					step_speed(velocidade do motor se é passo completo ou meio-passo).
 * saída: 	Não tém
 */
void maintenanceMotorSetupChange(stepState* state, uint8_t direction, uint8_t step_speed);

/*
 * Função para transição de estado conforme o sentido de rotação, velocidade do motor de passos Unipolar
 * entrada: stepState(estado do motor de passos unipolar passado como referência);
 * 					direction(direção de rotação do motor de passos unipolar);
 *					step_speed(velocidade do motor se é passo completo ou meio-passo).
 * saída: 	Não tém
 */
void motorStateTransition(stepState* state, uint8_t direction, uint8_t step_speed);

/*
 * Função responsável de controlar as tensões do GPIO do microcontrolado de acordo com estado do motor
 * de passos.
 * entrada: stepState(estado do motor de passos unipolar).
 * saída: 	Não tém
 */
void setStepMotorVoltage(stepState state);
/*------------------------------------------------------------------------------------------------------*/
#endif // end of file MOTOR_H
/*------------------------------------------------------------------------------------------------------*/