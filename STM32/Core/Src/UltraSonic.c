#include "main.h"
#include "UltraSonic.h"


void delay_us(uint32_t us)
{
	//利用CPU循环实现的非精准应用的微秒延时函数
	//使用HAL_RCC_GetHCLKFreq()函数获取主频值，经算法得1微秒的循环次数
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 8000000 * us);
    while (delay--); //循环delay次，达到1微秒延时
}

void Ultrasonic_Trig_L(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
}

void Ultrasonic_Trig_R(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	delay_us(20);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}
