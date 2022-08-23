/*
 * MOTOR.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Oscar Jin
 */

#include "MOTOR.h"

#define Confine_Value 9999	//定义PWM限幅值

//-------Param---------
uint16_t PWM1=5000,PWM2=5000;
//int8_t Select=0;
//-------Param---------

void motorC()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(100);
}

int16_t *amp_confine(int16_t MA, int16_t MB)
{
	static int16_t Confine_Motor[2];

	if(MA>=Confine_Value) MA=Confine_Value;
	if(MA<=-Confine_Value) MA=-Confine_Value;

	if(MB>=Confine_Value) MB=Confine_Value;
	if(MB<=-Confine_Value) MB=-Confine_Value;

	Confine_Motor[0]=MA;
	Confine_Motor[1]=MB;

	return Confine_Motor;
}
