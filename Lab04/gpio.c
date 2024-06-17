// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron

#include "gpio.h"

/*
extern uint8_t duty_cycle;
extern uint8_t motor_state;
extern uint8_t motor_rot;
*/

// -------------------------------------------------------------------------------
// Funcao GPIO_Init
// Inicializa os ports J e N
// Parametro de entrada: Nao tem
// Parametro de saida: Nao tem
void GPIO_Init(void)
{	
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTJ | GPIO_PORTQ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL);
	//1b.   apos isso verificar no PRGPIO se a porta esta pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL) ) != (GPIO_PORTJ | GPIO_PORTN | GPIO_PORTA | GPIO_PORTH | GPIO_PORTQ | GPIO_PORTP | GPIO_PORTF | GPIO_PORTE | GPIO_PORTK | GPIO_PORTM | GPIO_PORTL) ){}
	
	// 2. Limpar o AMSEL para desabilitar a analogica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	GPIO_PORTE_AHB_AMSEL_R |= 0x10; // PE4 analog; PE0, PE1 digital
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;
	GPIO_PORTE_AHB_PCTL_R = 0x00000000;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saida
	GPIO_PORTA_AHB_DIR_R = 0xF0;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTH_AHB_DIR_R = 0x0F;	// H3 ~ H0
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTQ_DIR_R = 0x0F;
	GPIO_PORTP_DIR_R = 0x20; // PP5
	GPIO_PORTF_AHB_DIR_R = 0x0C; // PF2, PF3
	GPIO_PORTE_AHB_DIR_R = 0x03; // PE4 input; PE0, PE1 output
	GPIO_PORTK_DIR_R = 0xFF;		// K7 ~ K0
	GPIO_PORTM_DIR_R = 0x07;		// M2 ~ M0
	GPIO_PORTL_DIR_R = 0x00;
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem funcao alternativa	
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
	GPIO_PORTE_AHB_AFSEL_R = 0x10;
	GPIO_PORTK_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0xF3;   	//Bit0 = entrada e bit1 = saida
	GPIO_PORTJ_AHB_DEN_R = 0x03;		//Bit0 e bit1
	GPIO_PORTH_AHB_DEN_R = 0x0F;		// PH3 ~ PH0
	GPIO_PORTN_DEN_R = 0x03; 		   	//Bit0 e bit1
	GPIO_PORTQ_DEN_R = 0x0F;
	GPIO_PORTP_DEN_R = 0x20;
	GPIO_PORTF_AHB_DEN_R = 0x0C;
	GPIO_PORTE_AHB_DEN_R = 0x03;
	GPIO_PORTF_AHB_DATA_R = 0x04;
	GPIO_PORTK_DEN_R = 0xFF;
	GPIO_PORTM_DEN_R = 0xF7;				// M7 ~ M4 TECLADO E M2 ~ M0 PARA LCD
	GPIO_PORTL_DEN_R = 0x0F;
	
	// Habilita leds de controle
	GPIO_PORTP_DATA_R = 0x20;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   	//bit0 e bit1
	GPIO_PORTM_PUR_R = 0xF0;
	GPIO_PORTL_PUR_R = 0x0F;
	
	PortJ_interrupt_init();
	
	ADC0_init();
	
	Timer0A_init();
}

// -------------------------------------------------------------------------------
// Funcao Timer0A_init
// Configura o timer 0
// Parametro de entrada: nao tem
// Parametro de saida: nao tem
void Timer0A_init(void)
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
	
	// Timeout de 79.999 clocks (period: 1ms)
	TIMER0_TAILR_R = 0x1387F;
	
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

void PortJ_interrupt_init(void)
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

void ADC0_init(void)
{
// Analog Digital Converter ====================================================
	// PF2 and PF3
	SYSCTL_RCGCADC_R = 0x01; // ADC0
	
	// Wait until ADC0 is ready
	while ((SYSCTL_PRADC_R & 0x01) != 0x01){}
	
	// Full convertion rate
	ADC0_PC_R = 0x5;
	
	// SS3 priority highest
	ADC0_SSPRI_R = 0x0123;
	
	// ASEN2 = 0
	ADC0_ACTSS_R &= ~0x0008;
	
	ADC0_EMUX_R &= ~0xF000;
	
	ADC0_SSMUX3_R = 9;
	
	ADC0_SSCTL3_R = 0x0006;
		
	ADC0_IM_R &= ~0x0008;
	
	ADC0_ACTSS_R |= 0x0008;
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

uint32_t PortE_Input(void)
{
	return GPIO_PORTE_AHB_DATA_R;
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

uint32_t adc_convertion(void)
{
	uint32_t var;
	ADC0_PSSI_R = ADC_PSSI_SS3;
	while ((ADC0_RIS_R & 0x08) == 0){}
	var = ADC0_SSFIFO3_R & 0xFFF;
	ADC0_ISC_R = 0x0008; // Clear Interrupt Flag
	return var;
}

void Timer0A_Handler(void)
{
		uint8_t rot_aux = motor_rot;
	// Limpar o flag de interrupcao
	TIMER0_ICR_R = 0x01;
	if (motor_state == OFF)
	{
		// Atualiza o timer da interrupcao
		TIMER0_TAILR_R = 80000 * duty_cycle / 100;
		motor_state = ON;
		if (motor_rot == CLOCKWISE)
			PortE_Output(1);
		else
			PortE_Output(2);
	}
	else
	{
		// Atualiza o timer da interrupcao
		TIMER0_TAILR_R = 80000 * (100 - duty_cycle) / 100;
		motor_state = OFF;
		PortE_Output(0);
	}
}


void GPIOPortJ_Handler(void)
{
	//
}
