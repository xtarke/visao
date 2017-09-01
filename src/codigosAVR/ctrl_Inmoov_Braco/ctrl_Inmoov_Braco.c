/* -----------------------------------------------------------------------------
 * Project:		Inmoov Servo Control
 * File:		ctrl_Inmoov_Braco.c
 * Author:		Sergio L Martins Jr
 * Created:		2017-03-22
 * Version:		Alfa
 * Purpose:		Programa para controlar multiplos servo motores.
 *				Separa os pwm's e os divide no tempo.
 *				Limitando a no maximo 4 servos.
 *
 *
 *		ToDo: Conferir se o canal ADC está no PWM correto
 *			  Calcular corretamente qual janela de tempo do ADC
 *			  Limitar valor de corrente máxima além da média máxima
 *			  Implementar pacotes para "setar" limites
 *			  Implementar gravação e leitura de corrente dos limites
 *			do servo em memória não volátil
 * -------------------------------------------------------------------------- */

// --------------------------------------------------------------------------------
//  System definitions ------------------------------------------------------------

#define F_CPU 20000000UL


// --------------------------------------------------------------------------------
//  Header files ------------------------------------------------------------------

#include "globalDefines.h"
#include "ATmega328.h"
#include "ctrl_Inmoov_Braco.h"

#include <string.h>

// --------------------------------------------------------------------------------
//  Project definitions -----------------------------------------------------------

enum {PWM_0, PWM_1, PWM_2, PWM_3};

enum {ACT_PWM0 = 2, ACT_PWM1, ACT_PWM2, ACT_PWM3};

// --------------------------------------------------------------------------------
//  New data types ----------------------------------------------------------------
typedef union flagsADC{
	struct{
		uint8 adcCount		: 3;	// Ctrl contagem - conta até 14
		uint8 adcChannel	: 2;	// Guarda informação qual channel selecionado
		uint8 unusedBits	: 3;
	};
	uint8 allFlags;
} flagsADC_t;

// --------------------------------------------------------------------------------
//  Global variables --------------------------------------------------------------
volatile uint8 flagsPWM;	// 2 LSBs - Maquina de estado ctrl PWM
							// bits 2, 3, 4 e 5 marcam se estão ativos
							// respectivamente PWM0, PWM1, PWM2, PWM3 - sendo 1 ativo

volatile uint16 pwmRegValue[4];
volatile uint8 receiveWd;

volatile uint16 adcCurrent;
volatile flagsADC_t ctrlADC;	// 3 LSB - Ctrl contagem - conta até 14
							// Bits 3 e 4 - Guarda Informação qual channel Selecionado

// --------------------------------------------------------------------------------
// 	Main Function -----------------------------------------------------------------

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

	uint8 buffer[4][BUFFER_SIZE]; // 4 buffer para armazenar os valores de 4 servos
	memset(buffer[PWM_0],0,BUFFER_SIZE);
	memset(buffer[PWM_1],0,BUFFER_SIZE);
	memset(buffer[PWM_2],0,BUFFER_SIZE);
	memset(buffer[PWM_3],0,BUFFER_SIZE);
	uint8 nextWrite[4];	// Aponta pro próximo local a ser escrito
	memset(nextWrite, 0, 4);
	uint16 totalBuffer[4];		// 16 bits são suficientes para até 64 valores de janela
	memset(totalBuffer, 0, 8);
	uint16 meanBuffer[4];
	memset(meanBuffer, 0, 8);
	uint8 aux8;

	// Global variables initialization
	flagsPWM = 0;			// Inicializa a maquina de estados do PWM no PWM_0
	//setBit(flagsPWM, 0);
	setBit(flagsPWM, ACT_PWM0);	// Inicia todos PWM como ativos
	setBit(flagsPWM, ACT_PWM1);
	setBit(flagsPWM, ACT_PWM2);
	setBit(flagsPWM, ACT_PWM3);

	// Led azul teste funcionamento
	// setBit(DDRB,PB1);
	// Pinos auxiliares P9
	setBit(PORTB, PB1);
	setBit(DDRD, PD2);
	setBit(DDRD, PD3);
	setBit(DDRD, PD4);

	// Inicialização valores dos servos
//	minCount[0] = 1300;
	minCount[0] = 900;
	maxCount[0] = 2100;
	minCount[1] = 1100;
	maxCount[1] = 2100;
	minCount[2] = 900;
	maxCount[2] = 2100;
	minCount[3] = 900;
	maxCount[3] = 2100;
	for(i=0; i<4;i++){
//		minCount[i] = 900;		// 900 e 2100 são valores em microSegundos
//		maxCount[i] = 2100;		// São os limites -60 graus e +60 graus
		percent[i] = 50;		// De inclinação dos servos utilizados
		aux16 = update_reg_value(minCount[i],maxCount[i],percent[i]);
	//	pwmRegValue[i] = aux16+PWM_INIT_VALUE+(PWM_PASSO*i);
	}
	// 3750 + 2500 + 12500*i
	pwmRegValue[0] = 6250;
	pwmRegValue[1] = 18750;
	pwmRegValue[2] = 31250;
	pwmRegValue[3] = 43750;

	// Receive
	receiveWd = 0;
	// ADC
	ctrlADC.allFlags = 0; 	// Seta o canal 0 do ADC


	// PWM initialization
	setBit(PWM0_DDR, PWM0_BIT);		// Seta pinos do pwm como saída
//clrBit(PWM0_PORT, PB0);
	setBit(PWM1_DDR, PWM1_BIT);
	setBit(PWM2_DDR, PWM2_BIT);
	setBit(PWM3_DDR, PWM3_BIT);

	// USART Configuration
	usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_38400, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
    //usartConfig(USART_MODE_ASYNCHRONOUS, USART_BAUD_9600, USART_DATA_BITS_8, USART_PARITY_NONE, USART_STOP_BIT_SINGLE);
	usartEnableReceiver();
	usartEnableTransmitter();
	usartActivateReceptionCompleteInterrupt();
	usartStdio();

	// Timer 0 	- ADC trigger
	timer0Config(TIMER_A_MODE_NORMAL, TIMER_A_PRESCALER_256);

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

	sei();
	while(1){
		// Recepção dos pacotes
		if(packageReady == 0){
			receivePackageData(&packageIndex, packageData, &packageSize, &packageReady);
			// Ativou Watchdog:
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

			uint8_t data;

			packageReady = 0;
			packageIndex = 0;

			switch(packageData[COMANDO]){
				case 0x01:	// packageData[0] = 0x01 - Posicionamento Servo
					percent[packageData[ID_SERVO]] = packageData[PCT_SERVO];

					aux16 = update_reg_value(minCount[packageData[ID_SERVO]],maxCount[packageData[ID_SERVO]],percent[packageData[ID_SERVO]]);
					pwmRegValue[packageData[ID_SERVO]] = aux16 + PWM_INIT_VALUE + (PWM_PASSO*packageData[ID_SERVO]);

					// Transmissão utilizando buildTransmitPackage
					packageAux[0] = packageData[0];		// Sem uso de for para maior velocidade
					packageAux[1] = packageData[1];
					packageAux[2] = packageData[2];
					packageSize = buildTransmitPackageData(packageData, packageAux, packageSize);
					i=0;
					while(i<packageSize){
						usartTransmit(packageData[i++]);
					}
//printf("-- PWM = %d pct = %d -- \n", packageData[1], percent[packageData[1]]);

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
					//packageSize = 4;
					while(i<packageSize){
//						cplBit(PORTD, PD2);
						usartTransmit(packageData[i++]);
					}
					break;
				case 0x13:	// packageData[1] = 0x13 - Leitura de corrente de todos os servos
					packageAux[0] = packageData[0];
					packageAux[1] = (uint8) (meanBuffer[ADC0_CH]);
					packageAux[2] = (uint8) (meanBuffer[ADC1_CH]);
					packageAux[3] = (uint8) (meanBuffer[ADC2_CH]);
					packageAux[4] = (uint8) (meanBuffer[ADC3_CH]);

					packageSize = buildTransmitPackageData(packageData, packageAux, 5);

					i=0;
					while(i<packageSize){
						usartTransmit(packageData[i++]);
					}
					break;

				case 0x14:
					packageAux[0] = packageData[0];
					data = packageData[1];

					if (data == 1){
						setBit(PORTB,PB1);

						setBit(PORTD, PD2);
						setBit(PORTD, PD3);
						setBit(PORTD, PD4);

					}
					else {
						clrBit(PORTB,PB1);

						clrBit(PORTD,PD2);
						clrBit(PORTD,PD3);
						clrBit(PORTD,PD3);
					}

					packageAux[0] = packageData[0];		
					packageAux[1] = packageData[1];
					packageAux[2] = packageData[2];
					packageSize = buildTransmitPackageData(packageData, packageAux, packageSize);
					i=0;
					//while(i<packageSize){
					//	usartTransmit(packageData[i++]);
					//}


					break;
			}

		} // End processamento pacotes

		// Processamento ADC
		if(ctrlADC.adcCount>=MEASURE_WINDOW){
			ctrlADC.adcCount=0;
			// Replace value in buffer and update total value
			totalBuffer[(ctrlADC.adcChannel)] -= buffer[(ctrlADC.adcChannel)][nextWrite[ctrlADC.adcChannel]];
			aux8 = (adcCurrent>>2);
			buffer[ctrlADC.adcChannel][nextWrite[ctrlADC.adcChannel]] = aux8;
			totalBuffer[ctrlADC.adcChannel] += buffer[ctrlADC.adcChannel][nextWrite[ctrlADC.adcChannel]];
			// Update pointer(?)
			nextWrite[ctrlADC.adcChannel] == (BUFFER_SIZE-1) ? nextWrite[ctrlADC.adcChannel]=0 : nextWrite[ctrlADC.adcChannel]++;
			// Get mean value
			meanBuffer[ctrlADC.adcChannel] = (totalBuffer[ctrlADC.adcChannel] >> DIV_VALUE);

			switch(ctrlADC.adcChannel){
			case ADC0_CH:
				ctrlADC.adcChannel = ADC1_CH;
				break;
			case ADC1_CH:
				ctrlADC.adcChannel = ADC2_CH;
				break;
			case ADC2_CH:
				ctrlADC.adcChannel = ADC3_CH;
				break;
			case ADC3_CH:
				ctrlADC.adcChannel = ADC0_CH;
				break;
			}
			adcSelectChannel(ctrlADC.adcChannel);
		}	// Fim do processamento do ADC

		// Testes dos limites de corrente
		if(meanBuffer[ctrlADC.adcChannel]>CURRENT_LIMIT_MAX){
			if(isBitSet(flagsPWM, ctrlADC.adcChannel+ACT_PWM0)){
				clrBit(flagsPWM, ctrlADC.adcChannel+ACT_PWM0);
			}
		}
		else if(meanBuffer[ctrlADC.adcChannel]<CURRENT_LIMIT_MIN){
			if(isBitClr(flagsPWM, ctrlADC.adcChannel+ACT_PWM0)){
				setBit(flagsPWM, ctrlADC.adcChannel+ACT_PWM0);
			}
		}

	}
}

// --------------------------------------------------------------------------------
//  Handlers ----------------------------------------------------------------------

USART_RECEIVER_BUFFER_FUNCTION_HANDLER;

ISR(ADC_vect)
{
	(ctrlADC.adcCount)++;
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
//printf("\n\r %d - %d - %d", isBitSet(flagsPWM, 1), isBitSet(flagsPWM, 0), isBitSet(flagsPWM, 0)&isBitClr(flagsPWM, 1));
	//setBit(PORTB, PB0);
//	uint8_t sreg = SREG;

//	timer1DeactivateCompareBInterrupt();
//	sei();



	if(isBitClr(flagsPWM, 0)&isBitClr(flagsPWM, 1)){		// 00 - PWM0
		if(isBitClr(PWM0_PIN, PWM0_BIT) & isBitSet(flagsPWM, ACT_PWM0)){
			setBit(PWM0_PORT, PWM0_BIT);
			timer1SetCompareBValue(pwmRegValue[PWM_0]);		// 2500+time
		}else{
			clrBit(PWM0_PORT, PWM0_BIT);
			setBit(flagsPWM, 0);	// Muda para PWM1
			timer1SetCompareBValue(15000);	// 15000
		}
	}
	else if(isBitSet(flagsPWM, 0)&isBitClr(flagsPWM, 1) & isBitSet(flagsPWM, ACT_PWM1)){	// 01 - PWM1
		if(isBitClr(PWM1_PIN, PWM1_BIT)){
			setBit(PWM1_PORT, PWM1_BIT);
			timer1SetCompareBValue(pwmRegValue[PWM_1]);		//
		}else{
			clrBit(PWM1_PORT, PWM1_BIT);
			setBit(flagsPWM, 1);	// 10 - PWM2
			clrBit(flagsPWM, 0);
			timer1SetCompareBValue(27500);	// 27500
		}
	}
	else if(isBitSet(flagsPWM, 1)&isBitClr(flagsPWM, 0) & isBitSet(flagsPWM, ACT_PWM2)){	// 10 - PWM2
		if(isBitClr(PWM2_PIN, PWM2_BIT)){
			setBit(PWM2_PORT, PWM2_BIT);
			timer1SetCompareBValue(pwmRegValue[PWM_2]);	// 27500 + time
		}else{
			clrBit(PWM2_PORT, PWM2_BIT);
			setBit(flagsPWM, 0);	// 11 - PWM3
			timer1SetCompareBValue(40000);
		}
	}
	else if(isBitSet(flagsPWM, 0)&isBitSet(flagsPWM, 1) & isBitSet(flagsPWM, ACT_PWM3)){	// 11 - PWM3
		if(isBitClr(PWM3_PIN, PWM3_BIT)){
			setBit(PWM3_PORT, PWM3_BIT);
			timer1SetCompareBValue(pwmRegValue[PWM_3]);
		}else{
			clrBit(PWM3_PORT, PWM3_BIT);
			clrBit(flagsPWM, 0);	// 00 - PWM0
			clrBit(flagsPWM, 1);
			timer1SetCompareBValue(2500);
		}
	}
//	clrBit(PORTB, PB0);



//	timer1ActivateCompareBInterrupt();
//	SREG = sreg;

}

ISR(TIMER2_OVF_vect)
{
	receiveWd = 1;
}
