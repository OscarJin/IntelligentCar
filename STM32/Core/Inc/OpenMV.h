/*
 * OpenMV_Decode.h
 *
 *  Created on: Aug 23, 2022
 *      Author: Oscar Jin
 */

#ifndef INC_OPENMV_H_
#define INC_OPENMV_H_

#include <string.h>
#include <stdlib.h>
#include "main.h"

typedef struct {
	uint8_t find_ball;
	int distance;
	float angle;
	uint8_t dir;	//L-0 R-1
	uint8_t find_green;
} ImageRecognitionRes;

ImageRecognitionRes Decode(uint8_t *str);

#endif /* INC_OPENMV_H_ */
