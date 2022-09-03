/*
 * OpenMV_Decode.c
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#include <OpenMV.h>
#include "math.h"
#define PI 3.1416

ImageRecognitionRes Decode(uint8_t *str)
{
    char* copy = malloc(12);
    strcpy(copy, (char*)str);
    ImageRecognitionRes res;

    res.find_ball = copy[0]-'0';
    char temp[6] = {""};
    strncpy(temp, copy+1, 4);
    res.distance = atoi(temp);

    strncpy(temp, copy+5, 5);
    res.angle = tan((float)atoi(temp) * PI / (100*180)) * res.distance;

//    res.dir = atoi(copy+10);
    res.dir = copy[10] - '0';

    res.find_green = copy[11] - '0';

	return res;
}
