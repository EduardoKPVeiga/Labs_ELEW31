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
		IMPORT	SWAP_BETWEEN_DISPLAY_AND_LEDS
		IMPORT	INICIALIZA_CONTROLE
		IMPORT	MOVER_LEDS

; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL 		PLL_Init                  	;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL 		SysTick_Init
	BL 		GPIO_Init                 	;Chama a subrotina que inicializa os GPIO
;	BL 		HABILITAR_LEDS
;	MOV 	R7,#3;
;	BL 		ACENDER_LED_ESTADO
	MOV		R7,	#2_11;					;Estado anterior da port J(verificar sw1(j0) e sw2(j1) mudaram de estados)
	MOV		R8, #7;						;Estado inicial das leds
	MOV		R9, #1						;Contador display estado crescente/decrescente
	MOV		R10, #0						;Contador display dezenas
	MOV		R11, #0						;Contador display unidades
	MOV		R12, #1						;Passo
;	BL		INICIALIZA_CONTROLE			;Acende as leds e deixa apagado o display
;	BL 		Fim

MainLoop
	PUSH	{LR}
	BL		VERIFICAR_BOTOES
	POP		{LR}
	BL		EXECUTAR_BUFFER_BOTAO
	BL		MOVER_LEDS
	BL 		Display_Count				;Chama a rotina para printar no display
	B		MainLoop

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
	
	PUSH	{LR}
	BL		ACENDER_LED_ESTADO
	POP		{LR}
	PUSH	{LR}
	MOV 	R0,#1;
	BL		SysTick_Wait1ms
	POP		{LR}
	
	PUSH	{LR}
	BL		VERIFICAR_BOTOES
	POP		{LR}
	
	ADD		R6, #1
	CMP		R6, #300						;Printa os numeros 300 vezes
	BCC		Print_Display_Count_Loop
	
	BX 		LR						 	;return

;--------------------------------------------------------------------------------
; Função Limit_Count_D
; Parâmetro de entrada: R11(contador das unidades)
; Parâmetro de saída: Não tem
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

;--------------------------------------------------------------------------------
; Função Limit_Count_D
; Parâmetro de entrada: R11(contador das dezenas)
; Parâmetro de saída: Não tem
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
;--------------------------------------------------------------------------------
; Função VERIFICAR_BOTOES
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
; VERIFICA O ESTADO DOS BOTOES, SE FOR DIFERENTE NÃO ATUALIZO
VERIFICAR_BOTOES
	PUSH	{LR}
	BL 		PortJ_Input;	R0 TEM O VALOR DA ENTRADA J
	POP		{LR}
	CMP		R0,#2_00000011
	BEQ		BUTTOM_BUFFER;	NAO FAÇO LEITURA DO BOTAO
	MOV		R7,R0;
BUTTOM_BUFFER
	BX		LR

; Função EXECUTAR_BUFFER_BOTAO
; Parâmetro de entrada: R7
; Parâmetro de saída: R7
; VERIFICA O ESTADO DOS BOTOES, SE FOR DIFERENTE NÃO ATUALIZO
EXECUTAR_BUFFER_BOTAO
	CMP		R7,#2_10
	BEQ		Incrementa_Passo
	CMP		R7,#2_01
	BEQ		Est_cres_ou_descres
	BX		LR
Incrementa_Passo
	ADD		R12,#1
	CMP 	R12,#10
	BCC		salto1
	MOV		R12,#0;
salto1
	MOV		R7,#2_11
	BX		LR
Est_cres_ou_descres
	CMP 	R9, #1
	BEQ 	DESCRESCENTE
	MOV		R9,	#1
	BX		LR
DESCRESCENTE
	MOV		R9, #0
	MOV		R7,#2_11
	BX		LR
;--------------------------------------------------------------------------------
; Função Fim (Fim do programa)
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
Fim
	NOP;
	
; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo
