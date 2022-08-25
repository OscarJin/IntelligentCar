/*
 * MOTOR.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Oscar Jin
 */

#include "MOTOR.h"

#define Confine_Value 999	//定义PWM限幅值

void motorC()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
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

void set_ccr(int16_t MA, int16_t MB)
{
	int16_t *Confine_Motor;
	Confine_Motor=amp_confine(MA, MB);

	if(Confine_Motor[0] >= 0)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);
		Confine_Motor[0] = -Confine_Motor[0];
	}

	if(Confine_Motor[1] >= 0)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
		Confine_Motor[1] = -Confine_Motor[1];
	}

	TIM8->CCR1 = Confine_Motor[0];
	TIM8->CCR2 = Confine_Motor[1];
}
