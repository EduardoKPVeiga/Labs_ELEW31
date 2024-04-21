; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; ========================
DISPLAY_U_EN	EQU	2_00100000
DISPLAY_D_EN	EQU	2_00010000

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms			
		IMPORT  GPIO_Init
        IMPORT  PortN_Output
        IMPORT  PortJ_Input	
		IMPORT 	ACENDER_LED_ESTADO
		IMPORT 	HABILITAR_LEDS
		IMPORT 	Print_Display


; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL 		PLL_Init                  	;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL 		SysTick_Init
	BL 		GPIO_Init                 	;Chama a subrotina que inicializa os GPIO
	BL 		HABILITAR_LEDS
;	MOV 	R7,#3;
;	BL 		ACENDER_LED_ESTADO
	MOV 	R2, #0;
	MOV 	R0, #DISPLAY_D_EN
	BL		Print_Display
	MOV		R9, #1						;Contador display estado crescente/decrescente
	MOV		R10, #0						;Contador display dezenas
	MOV		R11, #0						;Contador display unidades
	MOV		R12, #5						;Passo
	BL 		Fim

MainLoop
	BL 		Display_Count				;Chama a rotina para piscar LED
	BL 		PortJ_Input				 	;Chama a subrotina que lê o estado das chaves e coloca o resultado em R0
Verifica_Nenhuma
	CMP		R0, #2_00000001			 	;Verifica se nenhuma chave está pressionada
	BNE 	Verifica_SW1			 	;Se o teste viu que tem pelo menos alguma chave pressionada pula
	MOV 	R0, #0                   	;Não acender nenhum LED
	BL 		PortN_Output			 	;Chamar a função para não acender nenhum LED
	B 		MainLoop					;Se o teste viu que nenhuma chave está pressionada, volta para o laço principal
Verifica_SW1	
	CMP 	R0, #2_00000000			 	;Verifica se somente a chave SW1 está pressionada
	BNE 	MainLoop                 	;Se o teste falhou, volta para o início do laço principal
	B 		MainLoop                   	;Volta para o laço principal

;--------------------------------------------------------------------------------
; Função Display_Count
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
Display_Count
	CMP		R9, #1						;Testa se e contagem crescente ou decrescente
	BEQ		Increase_Display_Count
	BNE		Decrease_Display_Count
Increase_Display_Count
	ADD		R11, R12					;Incrementa o contador das unidades(R11) com o passo(R12)
	CMP		R11, #10					;Testa se o contador das unidades(R11) chegou no limite
	BCS		Limit_Count_U
	B		Print_Display_Count
Decrease_Display_Count
	SUB		R11, R12
	CMP		R11, #0					;Testa se o contador das unidades(R11) chegou no limite
	BLT		Limit_Count_U
	B		Print_Display_Count
Print_Display_Count
	MOV		R6, #0						;Contador para printar o mesmo numero varias vezes
Print_Display_Count_Loop
	MOV		R0, #DISPLAY_U_EN
	MOV		R2, R11
	PUSH	{LR}
	BL		Print_Display
	POP		{LR}
	PUSH 	{LR}
	MOV 	R0, #1	                	;Chamar a rotina de delay
	BL 		SysTick_Wait1ms
	POP 	{LR}
	
	MOV		R0, #DISPLAY_D_EN
	MOV		R2, R10
	PUSH	{LR}
	BL		Print_Display
	POP		{LR}
	PUSH 	{LR}
	MOV 	R0, #1	                	;Chamar a rotina de delay
	BL 		SysTick_Wait1ms
	POP 	{LR}
	ADD		R6, #1
	CMP		R6, #500						;Printa os numeros 500 vezes
	BCC		Print_Display_Count_Loop
	BX 		LR						 	;return

Limit_Count_U
	CMP		R9, #1						;Testa se e contagem crescente ou decrescente
	BEQ		Increase_Count_D
	BNE		Decrease_Count_D
Increase_Count_D
	SUB		R11, #10					;(Caso o passo seja maior que 1)
	ADD		R10, #1
	CMP		R10, #10					;Testa se o contador das dezenas(R10) chegou no limite
	BCS		Limit_Count_D
	B		Print_Display_Count
Decrease_Count_D
	ADD		R11, #10					;(Caso o passo seja menor que -1)
	SUB		R10, #1
	CMP		R10, #0						;Testa se o contador das dezenas(R10) chegou no limite
	BLT		Limit_Count_D
	B		Print_Display_Count
	
Limit_Count_D
	CMP		R9, #1						;Testa se e contagem crescente ou decrescente
	BEQ		Restart_Count_Asc
	BNE		Restart_Count_Desc
Restart_Count_Asc
	MOV		R11, #0						;Zera o contador das unidades(R11)
	MOV		R10, #0						;Zera o contador das dezenas(R10)
	B		Print_Display_Count
Restart_Count_Desc
	MOV		R11, #9						;Zera o contador das unidades(R11)
	MOV		R10, #9						;Zera o contador das dezenas(R10)
	B		Print_Display_Count
	

Fim
	NOP;
	
; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo
