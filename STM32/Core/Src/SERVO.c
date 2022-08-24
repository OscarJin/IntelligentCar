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
	int PulseWidth = 60;
	switch(mode)
	{
	case 0:
		PulseWidth = 200;
		while(PulseWidth>=60)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PulseWidth);
			PulseWidth-=10;
			HAL_Delay(100);
		}
		break;
	case 1:
		PulseWidth = 60;
		while(PulseWidth<=200)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, PulseWidth);
			PulseWidth+=10;
			HAL_Delay(100);
		}
		break;
	}
}

void Servo_Control_DOWN(uint8_t mode)
{
	//mode: 0-semi open, 1-full open, 2-semi close, 3-full close
	int PulseWidth;
	const int PulseClose = 60, PulseSemi=110, PulseFull=170;
	const int delay = 60;
	switch(mode)
	{
	case 0:
		PulseWidth = PulseClose;
		while(PulseWidth<=PulseSemi)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseWidth);
			PulseWidth+=10;
			HAL_Delay(delay);
		}
		break;
	case 1:
		PulseWidth = PulseClose;
		while(PulseWidth<=PulseFull)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseWidth);
			PulseWidth+=10;
			HAL_Delay(delay);
		}
		break;
	case 2:
		PulseWidth = PulseSemi;
		while(PulseWidth>=PulseClose)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseWidth);
			PulseWidth-=10;
			HAL_Delay(delay);
		}
		break;
	case 3:
		PulseWidth = PulseFull;
		while(PulseWidth>=PulseClose)
		{
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, PulseWidth);
			PulseWidth-=10;
			HAL_Delay(delay);
		}
		break;
	}
}
