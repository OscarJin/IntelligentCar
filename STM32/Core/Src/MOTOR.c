/*
 * MOTOR.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Oscar Jin
 */

#include "MOTOR.h"

//-------Param---------
uint16_t PWM1=3000,PWM2=3000,PWM3=200;
//int8_t Select=0;
//-------Param---------

void motorC()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(2000);
}
