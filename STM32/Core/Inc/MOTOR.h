/*
 * MOTOR.h
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f1xx_hal.h"

void motorC(void);
int16_t *amp_confine(int16_t MA, int16_t MB);
void set_ccr(int16_t MA, int16_t MB);
int16_t *read_encoder(void);

#endif /* INC_MOTOR_H_ */
