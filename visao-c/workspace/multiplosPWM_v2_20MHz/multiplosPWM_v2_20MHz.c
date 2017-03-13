/*
 * multiplosPWM_v2_20MHz.c
 *
 *  Created on: Mar 06, 2017
 *      Author: sergio
 */



/* -----------------------------------------------------------------------------
 * Project:		Multiplos PWM v2
 * File:		multiplosPWMv2.c
 * Author:		Sergio L Martins Jr
 * Created:		2017-03-06
 * Version:		1.0
 * Purpose:		Programa para controlar multiplos servo motores.
 *				Separa os pwm's e os divide no tempo. Limitado a no maximo 4 servos.
 *				Programa teste para 2 servos, Necessário expandir e deixar genérico
 *
 *				Adicionado o medidor de corrente por integração
 *
 *				Adicionar macros em função do clock F_CPU
 * -------------------------------------------------------------------------- */

// --------------------------------------------------------------------------------
//  System definitions ------------------------------------------------------------

#define F_CPU 20000000UL

// --------------------------------------------------------------------------------
//  Header files ------------------------------------------------------------------

#include "globalDefines.h"
#include "ATmega328.h"
#include "multiplosPWM_v2_20MHz.h"

#include <string.h>

//sudo /usr/bin/avrdude -pm328p -cusbasp -Uflash:w:multiplosPWM_v2.hex:a


// --------------------------------------------------------------------------------
//  Project definitions -----------------------------------------------------------
#define DATA_MAX_SIZE	50

// Servo1 - PB0
// Servo0 - PD7
// Servo2 - PD6
// Servo3 - PD5

#define PWM_REG_DDR		DDRD
#define PWM_REG_PORT	PORTD
#define PWM_REG_PIN		PIND
#define PWM0_BIT		PD5
#define PWM1_BIT		PD6

#define BUFFER_SIZE 64		// Se for 128 ou mais é necessário atentar ao valor máximo da váriavel de acumulo do adc
#define DIV_VALUE	6		// Valor para deslocamento e cálculo da média

#define MEASURE_WINDOW	7	// Controle da janela de acumulo e atualização do buffer
#define CURRENT_LIMIT	163	// Valor maximo do ADC, relativo a tensão

enum {HEAD_H, HEAD_V};

// --------------------------------------------------------------------------------
//  New Data Types ----------------------------------------------------------------

typedef union flagsPWM{
	struct{
		uint8 pwmState		: 2;	// Para saber em qual pwm está
		uint8 unusedBits	: 6;
	};
	uint8 allFlags;
} flagsPWM_t;

typedef union flagsADC{
	struct{
		uint8 adcCount		: 3;	// Ctrl contagem - conta até 14
		uint8 adcChannel	: 1;	// Guarda informação qual channel selecionado
		uint8 unusedBits	: 4;
	};
	uint8 allFlags;
} flagsADC_t;

// --------------------------------------------------------------------------------
//  Global variables --------------------------------------------------------------
volatile flagsPWM_t ctrlPwm;
uint16 pwmRegValue[4];
uint8 receiveWd;

volatile uint16 adcCurrent;
volatile flagsADC_t ctrlAdc;

// --------------------------------------------------------------------------------
// Functions ----------------------------------------------------------------------
void receivePackageData(uint8 *index, uint8 *data, uint8 *size, uint8 *ready);
uint8 buildTransmitPackageData(uint8 *packageData, uint8 *data, uint8 dataSize);

// --------------------------------------------------------------------------------
//  Main function -----------------------------------------------------------------

int main(void)
{
	// Local variables
	// Pwm variables
	uint16 minCount[4]; // Valores em microSegundos
	uint16 maxCount[4];
	uint8  percent[4];
	uint16 aux16;
	// Package variables
	uint8 packageData[DATA_MAX_SIZE];
	uint8 packageIndex = 0;
	uint8 packageReady = 0;
	uint8 packageSize = 0;
	uint8 packageAux[DATA_MAX_SIZE];
	uint8 i;

	uint8 buffer[2][BUFFER_SIZE]; // 2 buffer para armazenar os valores de 2 servos
	memset(buffer[HEAD_H],0,BUFFER_SIZE);
	memset(buffer[HEAD_V],0,BUFFER_SIZE);
	uint8 nextWrite[2];	// Aponta pro próximo local a ser escrito
	memset(nextWrite, 0, 2);
//	uint8 nextRead = 0;		// Desnecessário pq vai ser sempre escrito em cima
	uint16 totalBuffer[2];		// 16 bits são suficientes para até 64 valores de janela
	memset(totalBuffer, 0, 2);
	uint16 meanBuffer[2];
	memset(meanBuffer, 0, 2);
	uint8 aux8;

	// Global variables initialization
	ctrlPwm.allFlags = 0;
	setBit(ctrlPwm.allFlags, 0);
	// Nesse programa vou utilizar 2 servos
	for(i=0; i<2;i++){
		minCount[i] = 900;
		maxCount[i] = 2100;
		percent[i] = 50;
		aux16 = update_reg_value(minCount[i],maxCount[i],percent[i]);
		pwmRegValue[i] = aux16+PWM_INIT_VALUE+PWM_PASSO*i;
	}
	// Receive
	receiveWd = 0;
	// ADC
	adcCurrent = 0;
	ctrlAdc.adcChannel = HEAD_H;

	// PWM initialization
	setBit(PWM_REG_DDR, PWM0_BIT);		// Seta pinos do pwm como saída
	setBit(PWM_REG_DDR, PWM1_BIT);

	// USART Configuration
	usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_38400, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
	usartEnableReceiver();
	usartEnableTransmitter();
	usartActivateReceptionCompleteInterrupt();
	usartStdio();

	// Timer 0 	- ADC trigger
	//timer0SetCompareAValue(255);
	timer0Config(TIMER_A_MODE_NORMAL, TIMER_A_PRESCALER_1024);

	// Timer1 initialization
	timer1SetCompareAValue(PWM_TOP_VALUE);	//
	timer1SetCompareBValue(PWM_INIT_VALUE);
	timer1Config(TIMER_B_MODE_CTC, TIMER_A_PRESCALER_8);
	timer1ActivateCompareBInterrupt();

	//Timer2 initialization - WatchDog recepção
	timer2Config(TIMER_A_MODE_NORMAL, TIMER_B_PRESCALER_1024);	// Configura o timer
	timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_CLOCK_DISABLE);
	timer2SetCounterValue(0);
	timer2ActivateOverflowInterrupt();

	// ADC configuration
	adcConfig(ADC_MODE_AUTO_TIMER0_OVERFLOW, ADC_REFERENCE_POWER_SUPPLY, ADC_PRESCALER_128);
	adcDisableDigitalInput(0b000011);
	adcSelectChannel(ADC_CHANNEL_3);
	adcActivateInterrupt();
	adcEnable();

//setBit(DDRD, PD2);
//clrBit(PORTD, PD2);

//setBit(DDRB, PB1);

	sei();
    while(1){

    //	printf("-- %d --\n", meanBuffer[0]);

    	// Recepção dos pacotes
    	if(packageReady == 0){
			receivePackageData(&packageIndex, packageData, &packageSize, &packageReady);
if(receiveWd){
	receiveWd=0;

	//printf("-- WatchDog Recepção --\n");
	packageIndex= 0;
	packageSize = 0;
	packageReady = 0;
	timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_CLOCK_DISABLE);
	timer2SetCounterValue(0);
}
		}
    	// Processamento dos pacotes
		if(packageReady){
			// Maquina de tratamento do pacote

			packageReady = 0;
			packageIndex = 0;


			switch(packageData[0]){
				case 0x01:	// packageData[0] = 0x01 - Posicionamento Servo
					percent[packageData[1]] = packageData[2];

					aux16 = update_reg_value(minCount[packageData[1]],maxCount[packageData[1]],percent[packageData[1]]);
					pwmRegValue[packageData[1]] = aux16 + PWM_INIT_VALUE + (PWM_PASSO*packageData[1]);

					// Transmissão utilizando buildTransmitPackage
					packageAux[0] = packageData[0];		// Sem uso de for para maior velocidade
					packageAux[1] = packageData[1];
					packageAux[2] = packageData[2];
					packageSize = buildTransmitPackageData(packageData, packageAux, packageSize);
					i=0;
					while(i<packageSize){
						usartTransmit(packageData[i++]);
					}
printf("-- PWM = %d pct = %d -- \n", packageData[1], percent[packageData[1]]);

					// necessário ajustar para generico
					//				if(pwmRegValue[packageData[1]]<(minCount[packageData[1]]+2500)) pwmRegValue[packageData[1]]=minCount[packageData[1]];
					//				if(pwmRegValue[packageData[1]]>(maxCount[packageData[1]]+2500)) pwmRegValue[packageData[1]]=maxCount[packageData[1]];
					break;
				case 0x11:	// packageData[0] = 0x11 - Leitura de corrente
					// packageData[1] contém adress do servo a ser lido
					packageAux[0] = packageData[0];
					packageAux[1] = packageData[1];
					//packageAux[2] = (uint8)(meanBuffer[packageData[1]]>>8);
					//packageAux[3] = (uint8)(meanBuffer[packageData[1]]);
					packageAux[2] = (uint8) (meanBuffer[packageData[1]]);
					packageSize = buildTransmitPackageData(packageData, packageAux, 3);
					i=0;
					while(i<packageSize){
						usartTransmit(packageData[i++]);
					}
					break;
			}

		} // End processamento pacotes

		// Processamento ADC
		if(ctrlAdc.adcCount>=MEASURE_WINDOW){
//cplBit(PORTB, PB1);
			ctrlAdc.adcCount=0;
			// Replace value in buffer and update total value
			totalBuffer[(ctrlAdc.adcChannel)] -= buffer[(ctrlAdc.adcChannel)][nextWrite[ctrlAdc.adcChannel]];
			aux8 = (adcCurrent>>2);
			buffer[ctrlAdc.adcChannel][nextWrite[ctrlAdc.adcChannel]] = aux8;
			totalBuffer[ctrlAdc.adcChannel] += buffer[ctrlAdc.adcChannel][nextWrite[ctrlAdc.adcChannel]];
			// Update pointer(?)
			nextWrite[ctrlAdc.adcChannel] == (BUFFER_SIZE-1) ? nextWrite[ctrlAdc.adcChannel]=0 : nextWrite[ctrlAdc.adcChannel]++;
			// Get mean value
			meanBuffer[ctrlAdc.adcChannel] = (totalBuffer[ctrlAdc.adcChannel] >> DIV_VALUE);

			if(ctrlAdc.adcChannel == HEAD_H){
				ctrlAdc.adcChannel = HEAD_V;
				adcSelectChannel(ADC_CHANNEL_2);
			}else{
				ctrlAdc.adcChannel = HEAD_H;
				adcSelectChannel(ADC_CHANNEL_3);
			}

			// Debug
//			if(nextWrite[ctrlAdc.adcChannel]%10 == 0){
//				printf("adcCurrent = %u\n\r",adcCurrent);
//				printf("nextValue = %u\n\r",(uint8)(adcCurrent>>2));
//				printf("pointer = %u - totalBuffer = %u\n\r", nextWrite[ctrlAdc.adcChannel], totalBuffer[ctrlAdc.adcChannel]);
//				printf("Buffer[%d] = %u\n\r", ctrlAdc.adcChannel, buffer[ctrlAdc.adcChannel][nextWrite[ctrlAdc.adcChannel]]);
//				printf("media = %u\n\r", meanBuffer[ctrlAdc.adcChannel]);
//			}
		}
		if(meanBuffer[ctrlAdc.adcChannel]>CURRENT_LIMIT){
			// Limite de corrente atingido
		}
    }
}

// --------------------------------------------------------------------------------
//  Handlers ----------------------------------------------------------------------

USART_RECEIVER_BUFFER_FUNCTION_HANDLER;

ISR(ADC_vect)
{
	(ctrlAdc.adcCount)++;
	adcCurrent = ADC;

//	Seria feito assim se todas as medidas fossem analisadas (limitador de pico)
//	if(ctrlAdc.adcChannel == HEAD_H){
//		adcSelectChannel(ADC_CHANNEL_1);
//		ctrlAdc.adcChannel = HEAD_V;
//	}else{
//		adcSelectChannel(ADC_CHANNEL_0);
//		ctrlAdc.adcChannel = HEAD_H;
//	}

	setBit(TIFR0, TOV0);
}

ISR(TIMER1_COMPB_vect)
{
	if(isBitSet(ctrlPwm.allFlags, 0)){
		if(isBitClr(PWM_REG_PIN, PWM0_BIT)){
			setBit(PWM_REG_PORT, PWM0_BIT);
			timer1SetCompareBValue(pwmRegValue[0]);
		}else{
			clrBit(PWM_REG_PORT, PWM0_BIT);
			ctrlPwm.allFlags = ctrlPwm.allFlags<<1;
			timer1SetCompareBValue(PWM_PASSO+PWM_INIT_VALUE);
		}
	}
	else if(isBitSet(ctrlPwm.allFlags, 1)){
		if(isBitClr(PWM_REG_PIN, PWM1_BIT)){
			setBit(PWM_REG_PORT, PWM1_BIT);
			timer1SetCompareBValue(pwmRegValue[1]);
		}else{
			clrBit(PWM_REG_PORT, PWM1_BIT);
			ctrlPwm.allFlags = 1;
			timer1SetCompareBValue(PWM_INIT_VALUE);
		}
	}
}

ISR(TIMER2_OVF_vect)
{
	receiveWd = 1;
}

void receivePackageData(uint8 *index, uint8 *data, uint8 *size, uint8 *ready)
{
	uint8 aux8;
	uint8 i;

	while(!usartIsReceiverBufferEmpty()){
		aux8 = usartGetDataFromReceiverBuffer();
timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_PRESCALER_1024);
		switch(*index){
			case 0:		// Start delimiter
				if(aux8 == 0x7E){
					(*index)++;
timer2SetCounterValue(0);
setBit(PORTB, PB0);
				}
				break;
			case 1:		// Size
timer2SetCounterValue(0);
				*size = aux8;
				(*index)++;
				break;
			default:
timer2SetCounterValue(0);
				if(*index < ((*size) + 2)){
					data[(*index) - 2] = aux8;
					(*index)++;
				}else{
					for(i=0; i<(*index-2); i++){
						aux8 += data[i];
					}
					if(aux8 != 0xFF){
printf("-- Erro no CheckSum --\n");
						*ready = 0;
						*index = 0;
						*size = 0;
					}
					else{
						*ready = 1;
						*index = 0;
timer2Config(TIMER_A_MODE_NO_CHANGE, TIMER_B_CLOCK_DISABLE);
timer2SetCounterValue(0);
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
