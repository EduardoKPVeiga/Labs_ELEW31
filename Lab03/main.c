// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

//#include <stdint.h>
#include "motor.h"
#include "communication.h"
#include <string.h>

#define ON 									1
#define OFF 								0
#define UPDATE_ANGLE				6.13

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void print_Info_Motor(uint8_t revolutions, double angle);

int main(void)
{
	// variáveis de controle
	uint8_t jumps;																			// 	pular linhas
	uint8_t directionMotor = CLOCKWISE;									//	Sentido de rotação do motor.
	uint8_t	velocity = FULLSTEPMODE;										// 	Motor é meio passo ou passo completo.
	stepState	state = step_state_0;											// 	Máquina de estado para controlar o passo do motor de passo.
	uint8_t statusMotor = OFF;													// 	Motor esta ligado ou desligado
	double angle = 0.00;																//	Ângulo do motor de passos.
	double test = 365.35;
	uint8_t	revolutions;																// 	n.° de voltas do motor.
	char word[50], ch;
	uint8_t size;
	
	//Funções de inicialização 
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Uart_init();
	// loop principal
	while(1) {
		// Coletando os dados de configurações para funcionamento do Motor de Passos
		statusMotor = OFF;
		jumps = 0;
		clean_putty();
		move_cursor_beginning_putty();
		WriteWord("Enter = '/'");
		move_cursor_line_position(++jumps);
		WriteWord("Para canelar ou resetar digite: '*'");
		move_cursor_line_position(++jumps);
		WriteWord("Digite o numero de voltas: ");
		size = ReadWord(&word);
		if(!size) {
			continue;
		}
		revolutions = convertCharacterNumber(word);
		move_cursor_line_position(++jumps);
		WriteWord(word);
		move_cursor_line_position(++jumps);
		WriteWord("Para sentido Horario digite: 'H'");
		move_cursor_line_position(++jumps);
		WriteWord("Para sentido Anti-Horario digite: 'A'");
		move_cursor_line_position(++jumps);
		WriteWord("Digite o sentido de rotacao: ");
		size = ReadWord(&word);
		move_cursor_line_position(++jumps);
		WriteWord(word);
		move_cursor_line_position(++jumps);
		if(!size) {
			continue;
		}
		if(strcmp(word, "H") == 0) {
			directionMotor = CLOCKWISE;
		} else if (strcmp(word, "A") == 0){
			directionMotor = COUNTERCLOCKWISE;
		} else {
			continue;
		}
		WriteWord("Para Passo Completo digite: 'C'");
		move_cursor_line_position(++jumps);
		WriteWord("Para Meio Passo: 'M'");
		move_cursor_line_position(++jumps);
		WriteWord("Digite a Velocidade: ");
		size = ReadWord(&word);
		move_cursor_line_position(++jumps);
		WriteWord(word);
		move_cursor_line_position(++jumps);
		if(!size) {
			continue;
		}
		if(strcmp(word, "C") == 0) {
			velocity = FULLSTEPMODE;
		} else if (strcmp(word, "M") == 0){
			velocity = HALFSTEPMODE;
		} else {
			continue;
		}
		// Funcionando do motor, leitura será diferente
		statusMotor = ON;
		angle = 0.00;
		double aux = 0.00;
		while(statusMotor && revolutions) {													// SW1 teclando 
			motorStateTransition(&state, directionMotor, velocity);		//	muda para próximo estágio(passo) o motor
			setStepMotorVoltage(state);																// Coloca a Voltage pelo passo(estágio) atual
			SysTick_Wait1ms(STATE_TRANSITION_DELAY);
			switch(velocity) {
				case FULLSTEPMODE:
					angle += ANGLE_PER_STATE_FULLLSTEP;
					aux += ANGLE_PER_STATE_FULLLSTEP;
					break;
				case HALFSTEPMODE:
					angle += ANGLE_PER_STATE_HALFSTEP;
					aux += ANGLE_PER_STATE_HALFSTEP;
					break;
				default:
					angle = 0.00;
					break;
			}
			if(angle > 360.00) {
				angle = angle - 360.00;
				revolutions--;
			}
			
			// imprimir os dados na tela
			if(aux > UPDATE_ANGLE) {
				aux = 0.00;
				print_Info_Motor(revolutions, angle);
			}
		}
		print_Info_Motor(revolutions, angle);
		move_cursor_line_position(++jumps);
		WriteWord("FIM");
		move_cursor_line_position(++jumps);
		ch = ReadRX();
		while(ch != '*') {
			ch = ReadRX();
		}
	}
}

void print_Info_Motor(uint8_t revolutions, double angle) {
	// imprimir os dados na tela
			uint8_t jumps = 0;
			clean_putty();
			move_cursor_beginning_putty();
			WriteWord("Angulo: ");
			WriteTXDouble(angle, 2);
			WriteWord("\t\tVoltas: ");
			WriteTXNumber8(revolutions);
			//move_cursor_line_position(++jumps);
}

void acender_ledsN(uint8_t vn) {
	PortN_Output(vn);
}
void acender_ledsN(uint8_t vn) {
	PortN_Output(vn);
}void acender_ledsN(uint8_t vn) {
	PortN_Output(vn);
}
