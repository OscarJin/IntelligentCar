/*
 * PID.c
 *
 *  Created on: Aug 28, 2022
 *      Author: Oscar Jin
 */

#include "PID.h"
#include "main.h"
#include "MOTOR.h"

extern int PWM_L, PWM_R;

void Encoder_PID_init(PID* pid, float target)
{
	pid->Kp = 30;
	pid->Ki = 0;
	pid->Kd = 0;
	pid->target = target;
	pid->result = 600;
	pid->LastError = pid->PrevError = 0;
}

void PID_Calc(PID* pid, float measure)
{
	//Get error
	float ThisError = pid->target - measure;

	//Calculate Increment
	float PError = ThisError - pid->LastError;
	float IError = ThisError;
	float DError = ThisError - 2*pid->LastError + pid->PrevError;
	float delta = pid->Kp * PError + pid->Ki * IError + pid->Kd * DError;

	//Store Errors
	pid->PrevError = pid->LastError;
	pid->LastError = ThisError;
	pid->result += (int)delta;
}

//void PID_Dist(float dist, float Kp)
//{
//	int PWM_PID_Dist = (int) dist >= 80 ? 800 : dist * Kp;
//	set_ccr(PWM_PID_Dist, PWM_PID_Dist);
//}

