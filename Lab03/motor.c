// motor.c
// Arquivo de implementação do header motor.h
// Este arquivo em C tem as funçoes para controlar um Motor de Passos Unipolar
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 30/05/2024
// Versão: 1.0

#include "motor.h"

void maintenanceMotorSetupChange(stepState* state, uint8_t direction, uint8_t step_speed) {
	if(step_speed == FULLSTEPMODE) {
		if(*state % 2) {
			if(direction == CLOCKWISE) {
				*state = (*state + 1) % NUMBER_OF_MOTOR_STATES;
			} else {
				*state = (*state + NUMBER_OF_MOTOR_STATES - 1) % NUMBER_OF_MOTOR_STATES;
			}
		}
	}
}

void motorStateTransition(stepState* state, uint8_t direction, uint8_t step_speed) {
	uint8_t step_change = step_speed + 1;
	if(direction == CLOCKWISE) {
		*state = (*state + step_change) % NUMBER_OF_MOTOR_STATES;
	} else {
		*state = (*state + NUMBER_OF_MOTOR_STATES - step_change) % NUMBER_OF_MOTOR_STATES;
	}
}

void setStepMotorVoltage(stepState state) {
	uint8_t temp = (uint8_t)GPIO_PORTH_AHB_DATA_R;
	temp = temp | 0x0000000F;		// setando os 4 ultimos bits(H3 ~ H0)
	switch(state) {
		case 0:
			temp = temp & 0xFE;			// 1111 1110
			break;
		case 1:
			temp = temp & 0xFC;			// 1111 1100
			break;
		case 2:
			temp = temp & 0xFD;			// 1111 1101
			break;
		case 3:
			temp = temp & 0xF9;			// 1111 1001
			break;
		case 4:
			temp = temp & 0xFB;			// 1111 1011
			break;
		case 5:
			temp = temp & 0xF3;			// 1111 0011
			break;
		case 6:
			temp = temp & 0xF7;			// 1111 0111
			break;
		case 7:
			temp = temp & 0xF6;			// 1111 0110
			break;
		default:
			break;
	}
	GPIO_PORTH_AHB_DATA_R = temp;
}
