/*
 * OpenMV_Decode.c
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#include <OpenMV.h>

ImageRecognitionRes Decode(uint8_t *str)
{
    char* copy = malloc(11);
    strcpy(copy, (char*)str);
    ImageRecognitionRes res;

    res.find = copy[0];
    char temp[6] = {""};
    strncpy(temp, copy+1, 4);
    res.distance = atoi(temp);

    strncpy(temp, copy+5, 5);
    res.angle = (float)atoi(temp) / 100;

    res.dir = atoi(copy+10);

	return res;
}
