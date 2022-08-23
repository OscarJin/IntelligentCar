/*
 * OpenMV_Decode.c
 *
 *  Created on: 2022年8月23日
 *      Author: Oscar Jin
 */

#include "OpenMV_Decode.h"

ImageRecognitionRes *Decode(char* str)
{
    char* copy = malloc(20);
    strcpy(copy, str);
    ImageRecognitionRes *res = malloc(sizeof(ImageRecognitionRes));
    char c[] = "+";
	char *temp = strtok(copy, c);

    res->find = temp[0];
    temp = strtok(NULL, c);
    res->distance = atoi(temp);
    temp = strtok(NULL, c);
    res->angle = (float)atoi(temp) / 100;
	return res;
}
