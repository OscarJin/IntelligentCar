/*
 * MOTOR.h
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "main.h"

void motorC(void);
int16_t *amp_confine(int16_t MR, int16_t ML);
void set_ccr(int16_t MA, int16_t MB);
void read_encoder();
float EncoderCnt_to_Dist(int cnt);

#endif /* INC_MOTOR_H_ */
