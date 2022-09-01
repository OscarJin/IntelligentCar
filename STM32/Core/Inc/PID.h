/*
 * PID.h
 *
 *  Created on: Aug 28, 2022
 *      Author: Oscar Jin
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct{
	float target;
	int result;
	float Kp, Ki, Kd;
	float LastError;
	float PrevError;
} PID;

void Encoder_PID_init(PID* pid, int res, float target);

void PID_Calc(PID* pid, float encoder);
//void PID_Dist(float dist, float Kp);

#endif /* INC_PID_H_ */
