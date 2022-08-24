/*
 * Servo.h
 *
 *  Created on: Aug 24, 2022
 *      Author: Oscar Jin
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "stm32f1xx_hal.h"

void Servo_Control_UP(uint8_t mode);

void Servo_Control_DOWN(uint8_t mode);

#endif /* INC_SERVO_H_ */
