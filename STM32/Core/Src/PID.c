/*
 * PID.c
 *
 *  Created on: Aug 28, 2022
 *      Author: Oscar Jin
 */

#include "PID.h"
#include "main.h"

void Encoder_PID_init(PID* pid, int target)
{
	pid->Kp = 0;
	pid->Ki = 0;
	pid->Kd = 0;
	pid->target = target;
	pid->result = target;
	pid->LastError = pid->PrevError = 0;
}

void PID_Calc(PID* pid, int encoder)
{
	//Get error
	int ThisError = pid->target - encoder;

	//Calculate Increment
	int PError = ThisError - pid->LastError;
	int IError = ThisError;
	int DError = ThisError - 2*pid->LastError + pid->PrevError;
	int delta = pid->Kp * PError + pid->Ki * IError + pid->Kd * DError;

	//Store Errors
	pid->PrevError = pid->LastError;
	pid->LastError = ThisError;
	pid->result += delta;
}
