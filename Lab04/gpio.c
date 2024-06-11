// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>
#include "tm4c1294ncpdt.h"
#include "motor.h"

 // QPNMLKJHGFEDCBA
 // EDCBA9876543210
#define GPIO_PORTA  (0x0001) //bit 0
#define GPIO_PORTJ  (0x0100) //bit 8
#define GPIO_PORTQ	(0x4000) //bit 14
#define GPIO_PORTH	(0x0080) //bit 7 0000 0000 1000 0000
#define GPIO_PORTN  (0x1000) //bit 12
#define GPIO_PORTP	(0x2000) //bit 13
#define GPIO_PORTF	(0x0020) //bit 5
#define GPIO_PORTE	(0x0010) //bit 4

void SysTick_Wait1ms(uint32_t delay);
void GPIO_Init(void);
void Timer0A_init(void);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t valor);
void Timer0A_Handler(void);
void GPIOPortJ_Handler(void);
void PortJ_interrupt_init(void);
void ADC0_init(void);
void PortH_Output(uint8_t value);
void PortA_Output(uint32_t valor);
void PortQ_Output(uint32_t valor);
void PortE_Output(uint8_t value);

extern uint8_t duty_cycle;
extern uint8_t motor_state;
extern uint8_t motor_rot;

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports J e N
// Parametro de entrada: Nao tem
// Parametro de saida: Nao tem
void GPIO_Init(void)
{
	Timer0A_init();
	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTQ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE);
	//1b.   apos isso verificar no PRGPIO se a porta esta pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE) ){}
	
	// 2. Limpar o AMSEL para desabilitar a analogica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x0C;
	GPIO_PORTE_AHB_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;
	GPIO_PORTE_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTA_AHB_DIR_R = 0xF0;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTH_AHB_DIR_R = 0x0F;	// H3 ~ H0
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTQ_DIR_R = 0x0F;
	GPIO_PORTP_DIR_R = 0x20; // PP5
	GPIO_PORTF_AHB_DIR_R = 0x0C; // PF2, PF3
	GPIO_PORTE_AHB_DIR_R = 0x03; // PE0, PE1
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTF_AHB_AFSEL_R = 0x0C;
	GPIO_PORTE_AHB_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0xF3;   	//Bit0 = entrada e bit1 = saida
	GPIO_PORTJ_AHB_DEN_R = 0x03;		//Bit0 e bit1
	GPIO_PORTH_AHB_DEN_R = 0x0F;		// PH3 ~ PH0
	GPIO_PORTN_DEN_R = 0x03; 		   	//Bit0 e bit1
	GPIO_PORTQ_DEN_R = 0x0F;
	GPIO_PORTP_DEN_R = 0x20;
	GPIO_PORTF_AHB_DEN_R = 0x00;
	GPIO_PORTE_AHB_DEN_R = 0x03;
	
	// Habilita leds de controle
	GPIO_PORTP_DATA_R = 0x20;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   	//Bit0 e bit1
	
	PortJ_interrupt_init();
	
	ADC0_init();
}

// -------------------------------------------------------------------------------
// Funcao Timer0A_init
// Configura o timer 0
// Parametro de entrada: nao tem
// Parametro de saida: nao tem
void Timer0A_init()
{
	// Timer 0
	SYSCTL_RCGCTIMER_R = 0x1;
	
	// Espera o timer estar pronto
	while ((SYSCTL_PRTIMER_R & 0x01) != 0x01);
	
	// 0xFF.FF.FE.FE, desabilita o timer A
	TIMER0_CTL_R &= 0xFFFFFFFE;
	
	// Timer no modo 32 bits
	TIMER0_CFG_R = 0x0;
	
	// Modo periodico
	TIMER0_TAMR_R = 0x2;
	
	// Timeout de 7.999.999 clocks
	TIMER0_TAILR_R = 0x7A11FF;
	
	// Usado apenas para o prescale
	TIMER0_TAPR_R = 0x00;
	
	// Configuracao da interrupcao
	TIMER0_ICR_R = 0x1;
	
	// Interrupcao no timeout
	TIMER0_IMR_R = 0x00000001;
	
	// Prioridade da interrupcao
	NVIC_PRI4_R = NVIC_PRI4_R | ((uint32_t)4 << (uint32_t)29);
	
	// Habilita a interrupcao do timer
	NVIC_EN0_R = NVIC_EN0_R | ((uint32_t)1 << (uint32_t)19);

	// Habilita o timer A
	TIMER0_CTL_R |= 0x01;
}

void PortJ_interrupt_init()
{
	// Desabilita as interrupcoes na porta J
	GPIO_PORTJ_AHB_IM_R = 0x00;
	
	// Interrupcao por borda
	GPIO_PORTJ_AHB_IS_R = 0x00;
	
	// Borda unica em ambos os pinos
	GPIO_PORTJ_AHB_IBE_R = 0x00;
	
	// Borda de descida para J0 e de subida para J1
	GPIO_PORTJ_AHB_IEV_R = 0x02;
	
	// Realiza o ACK no registrador para ambos os pinos
	GPIO_PORTJ_AHB_ICR_R = 0x03;
	
	// Ativa a interrupcao em ambos os pinos
	GPIO_PORTJ_AHB_IM_R = 0x03;
	
	// Ativa a fonte de interrupcao no NVIC
	NVIC_EN1_R = (NVIC_EN1_R | (0x01 << 19));
	
	NVIC_PRI12_R = (NVIC_PRI12_R | 0x05 << 27);
}

void ADC0_init()
{
// Analog Digital Converter ====================================================
	// PF2 and PF3
	SYSCTL_RCGCADC_R = 0x01; // ADC0
	
	// Wait until ADC0 is ready
	while ((SYSCTL_PRADC_R & 0x01) != 0x01){}
	
	// Full convertion rate
	ADC0_PC_R = 0x7;
	
	// SS3 priority (highest)
	ADC0_SSPRI_R = 0x0000;
	
	// SS2 priority
	ADC0_SSPRI_R |= 0x1 << 8;
	
	// SS1 priority
	ADC0_SSPRI_R |= 0x2 << 4;
	
	// SS0 priority (lowest)
	ADC0_SSPRI_R |= 0x3;
	
	// ASEN3 = 0
	ADC0_ACTSS_R &= 0xF07; // 1111 0000 0111
	
	ADC0_EMUX_R = 0x00;
	
	ADC0_SSMUX3_R = 0x0;
	
	ADC0_SSCTL3_R = 0x6;
	
	// ASEN3 = 1
	ADC0_ACTSS_R |= 0x1 << 3;
}

// -------------------------------------------------------------------------------
// Funcao PortJ_Input
// Le os valores de entrada do port J
// Parametro de entrada: Nao tem
// Parametro de saida: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
}

// -------------------------------------------------------------------------------
// Funcao PortA_Output
void PortA_Output(uint32_t valor)
{
    uint32_t temp;
    temp = GPIO_PORTA_AHB_DATA_R & 0x0F; // ZERA 4 ULTIMOS BITS
    temp = temp | valor;
    GPIO_PORTA_AHB_DATA_R = temp; 
}

void PortQ_Output(uint32_t valor)
{
    uint32_t temp;
    temp = GPIO_PORTQ_DATA_R & 0xF0; // ZERA 4 PRIMEIROS BITS
    temp = temp | valor;
    GPIO_PORTQ_DATA_R = temp; 
}

void PortH_Output(uint8_t value) {
	uint8_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0xF0; // ZERA 4 ULTIMOS BITS
	temp = temp | value;
	GPIO_PORTH_AHB_DATA_R = temp;	
}


void PortE_Output(uint8_t value)
{
	uint8_t temp;
	temp = GPIO_PORTH_AHB_DATA_R & 0xFC; // ZERA OS 2 PRIMEIROS BITS
	temp = temp | value;
	GPIO_PORTE_AHB_DATA_R = temp;	
}

void Timer0A_Handler()
{
	// Limpar o flag de interrupcao
	TIMER0_ICR_R = 0x01;
	
	if (motor_state == OFF)
	{
		TIMER0_TAILR_R = 80000 * duty_cycle / 100;
		motor_state = ON;
		
		if (motor_rot == CLOCKWISE)
		{
			PortE_Output(1);
		}
		else
		{
			PortE_Output(2);
		}
	}
	else
	{
		TIMER0_TAILR_R = 80000 * (100 - duty_cycle) / 100;
		motor_state = OFF;
		
		PortE_Output(0);
	}
}


void GPIOPortJ_Handler()
{
	//
}
