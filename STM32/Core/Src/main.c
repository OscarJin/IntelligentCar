/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "MOTOR.h"
#include "SERVO.h"
#include "PID.h"
#include "BLUETOOTH.h"
#include "IMU.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BOUNDARY_THRESHOLD 15
#define BALL_ANGLE_THRESHOLD 60
#define BALL_BLIND_DIST 10
#define CRUISE_PWM 700
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/***电机***/

float EncoderDist_R;	//采样时间Distance
uint8_t EncoderDir_R;
float EncoderDist_L;
uint8_t EncoderDir_L;
PID EncoderPID_L, EncoderPID_R;
uint8_t Open_PID = 0;

/***超声***/
float Distance_L;
float Distance_R;

/***OpenMV***/
uint8_t OpenMV_Rxbuf[12];
ImageRecognitionRes ImgRes;

/***Motion Control***/
int State;
/*
 * 1-非边界转弯找�???????
 * 2-转弯对准�???????
 * 3-直行接近�???????
 * 4-抓球
 * 5-�???????
*/
int CatchBallSubState;
struct {
	float x;
	float y;
} Position;

int StateCnt = 0;
int CatchCnt = 0;	//State4 Counter

struct {
	ImageRecognitionRes img;
	float dist_L;
	float dist_R;
	float angle;
} CurrentInfo;

int TurnTimes = 0;

int BallCatch = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
//  Init_IMU();
  Encoder_PID_init(&EncoderPID_L, 1.5);
  Encoder_PID_init(&EncoderPID_R, 1.5);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM8_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_UART4_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  /*State Control 100ms*/
  HAL_TIM_Base_Start_IT(&htim7);

  /*舵机TIM1*/
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  Servo_Control_UP(0);
  Servo_Control_DOWN(2);
  Servo_Cam(0);

  /*电机TIM8*/
  HAL_TIM_Base_Start_IT(&htim8);
  HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_3);

  /*编码器TIM2 TIM4*/
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  /*编码器TIM2*/
  HAL_TIM_Base_Start_IT(&htim2);

  /*OpenMV USART1*/
  HAL_UART_Receive_IT(&huart1, OpenMV_Rxbuf, sizeof(OpenMV_Rxbuf));

  /*超声TIM3&6*/
  HAL_TIM_Base_Start_IT(&htim3);	//输入捕获
  HAL_TIM_Base_Start_IT(&htim6);	//定时中断

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  motorC();
	  if(StateCnt != 0)
	  {
		  //赋�??
		  CurrentInfo.img = ImgRes;
		  CurrentInfo.dist_L = Distance_L;
		  CurrentInfo.dist_R = Distance_R;
		  CurrentInfo.angle = Get_Angle_IMU();

		  if(CurrentInfo.img.find_ball == 0 && TurnTimes < 7/* && CurrentInfo.dist_L > BOUNDARY_THRESHOLD && CurrentInfo.dist_R > BOUNDARY_THRESHOLD*/)
			  State = 1;
		  else if(CurrentInfo.img.find_ball == 1 && CurrentInfo.img.angle > BALL_ANGLE_THRESHOLD)
			  State = 2;
		  else if(CurrentInfo.img.find_ball == 1 && CurrentInfo.img.angle <= BALL_ANGLE_THRESHOLD && CurrentInfo.img.distance > BALL_BLIND_DIST)
			  State = 3;
		  else if(CurrentInfo.img.find_ball == 1 && CurrentInfo.img.angle <= BALL_ANGLE_THRESHOLD && CurrentInfo.img.distance <= BALL_BLIND_DIST)
		  {
			  State = 4;
			  CatchBallSubState = 1;
		  }

		  switch(State)
		  {
		  case 1:
			  Open_PID = 0;
			  TurnTimes++;
			  set_ccr(-CRUISE_PWM, CRUISE_PWM);
			  StateCnt = 0;
			  break;
		  case 2:
			  Open_PID = 0;
			  if(CurrentInfo.img.dir == 0)	//ball on the left
				  set_ccr(CRUISE_PWM, -CRUISE_PWM);
			  else
				  set_ccr(-CRUISE_PWM, CRUISE_PWM);
			  StateCnt = 0;
			  break;
		  case 3:
			  Open_PID = 1;
			  StateCnt = 0;
			  break;
		  case 4:
			  Open_PID = 0;
			  switch(CatchBallSubState)
			  {
			  case 1:
				  set_ccr(0, 0);
				  if(CatchCnt < 6)
					  Servo_Control_DOWN(1);
				  else
				  {
					  CatchBallSubState = 2;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
			  case 2:
				  if(CatchCnt < 6)
					  Servo_Cam(1);
				  else
				  {
					  CatchBallSubState = 3;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
			  case 3:
				  if(CatchCnt < 6)
					  Open_PID = 1;
				  else
				  {
					  CatchBallSubState = 4;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
			  case 4:
				  if(CatchCnt < 5)
					  Servo_Control_DOWN(2);
				  else
				  {
					  Servo_Cam(0);
					  BallCatch++;
					  CatchBallSubState = 1;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
			  }
			  break;
		  }
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
