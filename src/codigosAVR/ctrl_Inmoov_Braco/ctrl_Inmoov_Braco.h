/*
 * ctrl_Inmoov_Braco.h
 *
 *  Created on: Mar 22, 2017
 *      Author: sergio
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#if F_CPU == 20000000
	#define PWM_TOP_VALUE  50000 // Top value to 20MHz
	#define PWM_PASSO 	   12500 // Passo entre cada PWM
	#define PWM_INIT_VALUE 2500
	// Macro para calculo do valor do registrador do PWM
	#define PWM_PRODUTO(VALUE)	((VALUE<<1)+(VALUE>>1))	// Multiplica por 2,5
#else
	#if F_CPU == 16000000
		#define PWM_TOP_VALUE  40000 // Top value to 16MHz
		#define PWM_PASSO 	   10000 // Passo entre cada PWM
		#define PWM_INIT_VALUE 2000
		#define PWM_PRODUTO(VALUE)	(VALUE<<1)	// Multiplica por 2,0
	#else
		#error Error 200 - Valor de F_CPU Invalido.
	#endif
#endif

// --------------------------------------------------------------------------------
//  Project definitions -----------------------------------------------------------

// Protocolo de comunicação

#define COMANDO		0
#define ID_SERVO	1
#define PCT_SERVO	2

// Reception Definitions

#define DATA_MAX_SIZE	50

// PWM Definitions

// Servo1 - PB0
// Servo0 - PD7
// Servo2 - PD6
// Servo3 - PD5

#define PWM0_DDR	DDRD
#define PWM0_PORT	PORTD
#define PWM0_PIN	PIND
#define PWM0_BIT	PD7

#define PWM1_DDR	DDRB
#define PWM1_PORT	PORTB
#define PWM1_PIN	PINB
#define PWM1_BIT	PB0

#define PWM2_DDR	DDRD
#define PWM2_PORT	PORTD
#define PWM2_PIN	PIND
#define PWM2_BIT	PD6

#define PWM3_DDR	DDRD
#define PWM3_PORT	PORTD
#define PWM3_PIN	PIND
#define PWM3_BIT	PD5

// Current Measurement definitions
#define BUFFER_SIZE 64		// Se for 128 ou mais é necessário atentar ao valor máximo da váriavel de acumulo do adc
#define DIV_VALUE	6		// Valor para deslocamento e cálculo da média

#define MEASURE_WINDOW	7	// Controle da janela de acumulo e atualização do buffer
#define CURRENT_LIMIT_MAX	163	// Valor maximo do ADC, antes de desligar PWM
#define CURRENT_LIMIT_MIN	40	// Valor minimo do ADC, antes de religar PWM

#define ADC0_CH 1
#define ADC1_CH 2
#define ADC2_CH 0
#define ADC3_CH 3

// --------------------------------------------------------------------------------
// Functions ----------------------------------------------------------------------





#endif /* HARDWARE_H_ */
