/*
 * comm.h
 *
 *  Created on: Sep 2, 2017
 *      Author: xtarke
 */

#ifndef COMM_H_
#define COMM_H_

#include <stdint.h>

uint16 update_reg_value(uint16 min, uint16 max, uint8 percent);
uint8 buildTransmitPackageData(uint8 *packageData, uint8 *data, uint8 dataSize);
void receivePackageData(uint8 *index, uint8 *data, uint8 *size, uint8 *ready);


#endif /* COMM_H_ */
