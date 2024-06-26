// motor.c
// Arquivo de implementacao do header motor.h
// Este arquivo em C tem as funcoes para controlar um Motor de Passos Unipolar
// Autores: Fernando Abreu e Eduardo Veiga
// Data: 30/05/2024
// Versao: 1.0

#include "motor.h"

volatile uint8_t duty_cycle = (uint8_t)100;
volatile uint8_t motor_state = OFF;
volatile uint8_t motor_rot = CLOCKWISE;