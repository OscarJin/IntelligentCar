/*
 * BLUETOOTH.h
 *
 *  Created on: 2022年8月25日
 *      Author: 杨朔宁
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "main.h"
#include "stdio.h"

extern UART_HandleTypeDef huart2;

void SendInt(int a);
void SendComma(void);
void SendEnter(void);


#endif /* INC_BLUETOOTH_H_ */
