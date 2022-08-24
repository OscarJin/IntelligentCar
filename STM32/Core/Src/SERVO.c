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
	//mode: 0-0, 1-120
	switch(mode)
	{
	case 0:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 100);
		break;
	case 1:
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 200);
		break;
	}
}
