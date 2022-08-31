/*
 * BLUETOOTH.c
 *
 *  Created on: Aug 30, 2022
 *      Author: Oscar Jin
 */

#include "BLUETOOTH.h"

void SendInt(int a)
{
	uint8_t str[10]="";
	itoa(a, (char*)str, 10);
	//HAL_UART_Transmit_IT(&huart2,str,10);
	HAL_UART_Transmit(&huart2, str, 5, 20);
	SendEnter();
    //HAL_Delay(1);
}

void SendEnter(void)
{
	uint8_t ter[2]="\n\r";
	HAL_UART_Transmit(&huart2,ter,5,20);
}
