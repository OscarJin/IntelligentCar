/*
 * BLUETOOTH.c
 *
 *  Created on: Aug 30, 2022
 *      Author: Oscar Jin
 */

#include "BLUETOOTH.h"

void SendInt(int a)
{
	char str[10];
	char space[5]=" ";
	itoa(a,str,10);
	//HAL_UART_Transmit_IT(&huart2,str,10);
	HAL_UART_Transmit(&huart2,str,5,20);
    //HAL_Delay(1);
}

void SendComma(void)
{
	uint8_t coo[5]=",";
	//HAL_UART_Transmit_IT(&huart2,coo,2);
	HAL_UART_Transmit(&huart2,coo,5,20);
	//HAL_Delay(1);
}

void SendEnter(void)
{
	uint8_t ter[5]="\n\r";
	HAL_UART_Transmit(&huart2,ter,5,20);
//HAL_UART_Transmit_IT(&huart2,ter,3);
//HAL_Delay(1);
}
