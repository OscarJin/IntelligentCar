/*
 * Servo.c
 *
 *  Created on: Aug 24, 2022
 *      Author: Oscar Jin
 */

#include <SERVO.h>

extern TIM_HandleTypeDef htim1;

void Servo_Control_UP(uint8_t mode)
{
	//mode: 0-close, 1-open
	switch(mode)
	{
	case 0:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 80);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 200);
		break;
	}
}

void Servo_Control_DOWN(uint8_t mode)
{
	//mode: 0-semi open, 1-full open, 2-close
	const int PulseClose = 80, PulseSemi=150, PulseFull=200;
	switch(mode)
	{
	case 0:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseSemi);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseFull);
		break;
	case 2:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseClose);
		break;
	}
}


void Dump()
{
	Servo_Control_DOWN(0);
	HAL_Delay(500);
	Servo_Control_UP(1);
	HAL_Delay(3000);
	Servo_Control_UP(0);
	Servo_Control_DOWN(2);
}

void Servo_Cam(uint8_t mode)
{
	//mode: 0-远，1-近
	switch(mode)
	{
	case 0:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 105);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 140);
		break;
	}
}
