/*
 * PID.h
 *
 *  Created on: Aug 28, 2022
 *      Author: Oscar Jin
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct{
	int target;
	int result;
	int Kp, Ki, Kd;
	int LastError;
	int PrevError;
} PID;

void Encoder_PID_init(PID* pid, int target);

void PID_Calc(PID* pid, int encoder);

#endif /* INC_PID_H_ */
