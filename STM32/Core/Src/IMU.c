/*
 * IMU.c
 *
 *  Created on: Aug 30, 2022
 *      Author: Oscar Jin
 */

#include "IMU.h"

Angle stcAngle;

char YAWCMD[3] = {0XFF,0XAA,0X52};
// send command to get angle of each direction [x,y,z]

//Init IMU
void Init_IMU()
{
	HAL_Delay(10000);
//	SendCmd(YAWCMD);
}

//command IMU to send angle to STM32
void SendCmd(char cmd[])
{
	char i;
	for(i = 0; i < 3; i++)
	{
		while((UART4->SR & USART_SR_TXE) == 0);
		UART4->DR = cmd[i];
	}
}

float Get_Angle_IMU()
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;
	int z = -400 ;
	while(z == -400)
	{
		while((UART4->SR & USART_SR_RXNE)==0);
		ucRxBuffer[ucRxCnt++] = UART4->DR;
		if (ucRxBuffer[0] != 0x55) //数据头不对，则重新开始寻找0x55数据头
			ucRxCnt=0;
		else
		{
			if(ucRxCnt>10)
			{
				if(ucRxBuffer[1]==0x53)
				{
					memcpy(&stcAngle,&ucRxBuffer[2],8);
					z = (stcAngle.Angle[2]) * 180 / 32768;
				}
			ucRxCnt=0;
			}
		}
	}
	return z;
}
