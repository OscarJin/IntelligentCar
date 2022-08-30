#ifndef __JY901_H
#define __JY901_H

#include "main.h"
#include "string.h"

extern UART_HandleTypeDef huart4;

typedef struct
{
	short Angle[3];
	short T;
}Angle;

void Init_IMU();	//Init IMU
void SendCmd(char cmd[]);	//IMU send angle to STM32
float Get_Angle_IMU();

#endif
