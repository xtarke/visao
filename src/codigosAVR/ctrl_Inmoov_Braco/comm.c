/*
 * comm.c
 *
 *  Created on: Sep 2, 2017
 *      Author: xtarke
 */

#include "globalDefines.h"
#include "ATmega328.h"
#include "hardware.h"


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

uint16 update_reg_value(uint16 min, uint16 max, uint8 percent)
{
	uint16 aux16;

	aux16 = (max-min)/100;
	aux16 = aux16*percent;
	aux16 = aux16 + min;
	aux16 = PWM_PRODUTO(aux16);

	return aux16;
}
