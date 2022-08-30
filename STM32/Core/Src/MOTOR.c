/*
 * MOTOR.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Oscar Jin
 */

#include "MOTOR.h"


#define Confine_Value 999	//定义PWM限幅值

extern int EncoderCnt_L;
extern uint8_t EncoderDir_L;
extern int EncoderCnt_R;
extern uint8_t EncoderDir_R;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

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

void set_ccr(int16_t MR, int16_t ML)
{
	//MA-R MB-L
	int16_t *Confine_Motor;
	Confine_Motor=amp_confine(MR, ML);

	if(Confine_Motor[0] <= 0)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
		Confine_Motor[0] = -Confine_Motor[0];
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);
	}

	if(Confine_Motor[1] <= 0)
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);
		Confine_Motor[1] = -Confine_Motor[1];
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET);
	}

	TIM8->CCR1 = Confine_Motor[0];
	TIM8->CCR2 = Confine_Motor[1];
}

void read_encoder()
{
	EncoderCnt_R = __HAL_TIM_GET_COUNTER(&htim2);
	EncoderDir_R = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);
	__HAL_TIM_SET_COUNTER(&htim2, 0);

	EncoderCnt_L = __HAL_TIM_GET_COUNTER(&htim4);
	EncoderDir_L = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim4);
	__HAL_TIM_SET_COUNTER(&htim4, 0);
}
