/*
 * OpenMV_Decode.c
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#include <OpenMV.h>

ImageRecognitionRes Decode(uint8_t *str)
{
    char* copy = malloc(20);
    strcpy(copy, (char*)str);
    ImageRecognitionRes res;
    char c[] = "+";
	char *temp = strtok(copy, c);

    res.find = temp[0];
    temp = strtok(NULL, c);
    res.distance = atoi(temp);
    temp = strtok(NULL, c);
    res.angle = (float)atoi(temp) / 100;
	return res;
}
