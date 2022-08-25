/*
 * BLUETOOTH.h
 *
 *  Created on: 2022年8月25日
 *      Author: 杨朔宁
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_
extern UART_HandleTypeDef huart2;
#include "main.h"
#include "stdio.h"
void append(char s[],char c)
{
  int i,j;
  for(i=0;s[i]!='\0';i++)
  {
       if(s[i]==c)
       {
           for(j=i;s[j]!='\0';j++)
            s[j]=s[j+1];
       }
  }
}
void transD(int a)
{
char str[10];
char space[5]=" ";
	itoa(a,str,10);
	append(str,space);
	HAL_UART_Transmit_IT(&huart2,str,10);
	HAL_Delay(500);
}
void transC(void)
{uint8_t coo[5]=",";
HAL_UART_Transmit(&huart2,coo,2,1000);
HAL_Delay(1000);
}

void transR(void)
{uint8_t ter[5]="\n\r";
HAL_UART_Transmit(&huart2,ter,3,1000);
HAL_Delay(1000);
}

#endif /* INC_BLUETOOTH_H_ */
