/*
 * ctrl_Inmoov_Braco.h
 *
 *  Created on: Mar 22, 2017
 *      Author: sergio
 */

#ifndef CTRL_INMOOV_BRACO_H_
#define CTRL_INMOOV_BRACO_H_

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

#define PWM1_DDR	DDRD
#define PWM1_PORT	PORTD
#define PWM1_PIN	PIND
#define PWM1_BIT	PD7

#define PWM0_DDR	DDRB
#define PWM0_PORT	PORTB
#define PWM0_PIN	PINB
#define PWM0_BIT	PB0

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


#define ADC0_CH 3
#define ADC1_CH 2
#define ADC2_CH 1
#define ADC3_CH 0

// --------------------------------------------------------------------------------
// Functions ----------------------------------------------------------------------

uint16 update_reg_value(uint16 min, uint16 max, uint8 percent)
{
	uint16 aux16;

	aux16 = (max-min)/100;
	aux16 = aux16*percent;
	aux16 = aux16 + min;
	aux16 = PWM_PRODUTO(aux16);

	return aux16;
}

void receivePackageData(uint8 *index, uint8 *data, uint8 *size, uint8 *ready)
{
	uint8 aux8;
	uint8 i;

	while(!usartIsReceiverBufferEmpty()){
		aux8 = usartGetDataFromReceiverBuffer();
//timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_PRESCALER_1024);
		switch(*index){
			case 0:		// Start delimiter
				if(aux8 == 0x7E){
					(*index)++;
//timer2SetCounterValue(0);
//setBit(PORTB, PB0);
				}
				break;
			case 1:		// Size
//timer2SetCounterValue(0);
				*size = aux8;
				(*index)++;
				break;
			default:
//timer2SetCounterValue(0);
				if(*index < ((*size) + 2)){
					data[(*index) - 2] = aux8;
					(*index)++;
				}else{
					for(i=0; i<(*index-2); i++){
						aux8 += data[i];
					}
					if(aux8 != 0xFF){
//printf("-- Erro no CheckSum --\n");
//usartTransmit(0x0F);
//usartTransmit(0x0F);
//usartTransmit(0x0F);
						*ready = 0;
						*index = 0;
						*size = 0;
					}
					else{
						*ready = 1;
						*index = 0;
//timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_CLOCK_DISABLE);
//timer2SetCounterValue(0);
					}
				}
				break;
		} // end switch(packageIndex)
	} // end while(!usartIsReceiverBufferEmpty())
}

// packageData - Local onde dados serão armazenados
// id - O ID do pacote recebido acrescido de 0x7F
// data - Os dados do pacote (eco)
// dataSize - Tamanho dos dados a serem enviados
// retorno - Tamanho do pacote montado
uint8 buildTransmitPackageData(uint8 *packageData, uint8 *data, uint8 dataSize)
{
	uint8 size=2; // 0 é 0x7E e 1 é size
	uint8 i, checkSum;

	packageData[0] = 0x7E;
	packageData[size++] = data[0] + 0x7F;
	for(i=1; i<dataSize; i++){
		packageData[size++] = data[i];
	}
	checkSum = 0xFF;
	for(i=2; i<size; i++){
		checkSum -= packageData[i];
	}
	packageData[size++] = checkSum-0x7E;
	packageData[1] = size-3;	// Lenght

	return size;
}



#endif /* CTRL_INMOOV_BRACO_H_ */
