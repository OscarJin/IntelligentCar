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
#define BOUNDARY_THRESHOLD 25
#define BALL_ANGLE_THRESHOLD 7.5
#define BALL_BLIND_DIST 18
#define BALL_FAR_DIST 60
#define CRUISE_PWM 550
#define TURN_PWM 720
#define DUMP_THRESHOLD 20
#define CRUISE_VELOCITY 0.2
#define TURN_VELOCITY 0.25
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
uint8_t Open_PID = 0;	//0-Close, 1-Straight, 2-TurnRight 3-TurnLeft

/***超声***/
float Distance_L;
float Distance_R;

/***OpenMV***/
uint8_t OpenMV_Rxbuf[13];
ImageRecognitionRes ImgRes;

/***Motion Control***/
int State;
/*
 * 1-Find ball no boundary
 * 2-turn and target
 * 3-approach
 * 4-catch
 * 5-?
 * 6-Return
 * 7-Obstacle, back
*/
int FindBallSubState = 0;	//State1 Sub
int TargetSubState = 0;	//State2 Sub
int CatchBallSubState = 1;	//State4 Sub
int ReturnSubState = 1;	//State6 Sub
int FindGreenSubState = 0;	//State 6 Find Sub


//struct {
//	float x;
//	float y;
//} Position;

int StateCnt = 0;
int FindCnt = 0;	//State1 Counter
int TargetCnt = 0;	//State 2 Counter
int CatchCnt = 0;	//State4 Counter
int ReturnCnt = 0;	//State6 Counter
int ObstacleCnt = 0;	//State7 Counter

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
	  motorC();
	  if(StateCnt != 0)
	  {
		  //car state
		  CurrentInfo.img = ImgRes;
		  CurrentInfo.dist_L = Distance_L;
		  CurrentInfo.dist_R = Distance_R;
		  CurrentInfo.angle = Get_Angle_IMU();
		  if(CurrentInfo.angle >= -33 && CurrentInfo.angle <= 147)
			  CurrentInfo.angle += 33;
		  else
			  CurrentInfo.angle -= 327;

		  set_ccr(0, 0);
		  Open_PID = 0;

//		  State = 6;
		  BallCatch = 10;
#if 1
		  if(CatchBallSubState != 1)
			  State = 4;
		  else if(CurrentInfo.img.find_ball == 0  /*&& CurrentInfo.dist_L > BOUNDARY_THRESHOLD && CurrentInfo.dist_R > BOUNDARY_THRESHOLD*/)
		  {
			  State = 1;
			  CatchBallSubState = 1;
		  }
		  else if(CurrentInfo.img.find_ball == 1 && (CurrentInfo.img.angle > BALL_ANGLE_THRESHOLD && CurrentInfo.img.distance < BALL_FAR_DIST) )
		  {
			  State = 2;
			  CatchBallSubState = 1;
		  }
		  else if(CurrentInfo.img.find_ball == 1 && CurrentInfo.img.angle <= BALL_ANGLE_THRESHOLD && CurrentInfo.img.distance <= BALL_BLIND_DIST+5)
		  {
			  State = 4;
		  //			  CatchBallSubState = 1;
		  }
		  else if(CurrentInfo.img.find_ball == 1 && ((CurrentInfo.img.angle <= BALL_ANGLE_THRESHOLD && CurrentInfo.img.distance > BALL_BLIND_DIST) || CurrentInfo.img.distance >= BALL_FAR_DIST))
		  {
			  State = 3;
			  CatchBallSubState = 1;
		  }

		  if(BallCatch >= 9)
		  {
			  State = 6;
			  CatchBallSubState = 1;
			  if(CurrentInfo.img.find_green == 0)
			  {
				  ReturnSubState = 1;
			  }
			  else if(CurrentInfo.img.find_green == 1 /*&& (CurrentInfo.dist_L <= BOUNDARY_THRESHOLD || CurrentInfo.dist_R <= BOUNDARY_THRESHOLD)*/)
			  {
				  ReturnSubState = 3;
			  }
			  else
			  {
				  ReturnSubState = 2;
			  }
		  }
		  else if(CurrentInfo.img.find_ball == 0 && (CurrentInfo.dist_L <= BOUNDARY_THRESHOLD || CurrentInfo.dist_R <= BOUNDARY_THRESHOLD) && BallCatch < 9)
		  {
			  State = 7;
			  CatchBallSubState = 1;
		  }

		  SendInt(CurrentInfo.img.find_green);
#endif

		  switch(State)
		  {
		  case 1:	//State 1
			  Servo_Cam(0);
//			  TurnTimes++;
			  if(FindBallSubState == 0)
			  {
				  if(FindCnt == 0)
				  {
					  Encoder_PID_init(&EncoderPID_L, TURN_PWM, TURN_VELOCITY);
					  Encoder_PID_init(&EncoderPID_R, TURN_PWM, TURN_VELOCITY);
				  }
				  set_ccr(-TURN_PWM, TURN_PWM);
				  Open_PID = 2;
				  if(FindCnt++ == 3)
				  {
					  FindCnt = 0;
					  FindBallSubState = 1;
				  }
			  }
			  else
			  {
				  set_ccr(0, 0);
				  Open_PID = 0;
				  if(FindCnt++ == 3)
				  {
				  	  FindCnt = 0;
				  	  FindBallSubState = 0;
				  }
			  }
			  StateCnt = 0;
			  break;
		  case 2:	//State 2
			  Servo_Cam(0);
			  if(TargetSubState == 0)
			  {
				  if(TargetCnt == 0)
				  {
					  Encoder_PID_init(&EncoderPID_L, TURN_PWM, TURN_VELOCITY);
					  Encoder_PID_init(&EncoderPID_R, TURN_PWM, TURN_VELOCITY);
				  }

				  if(CurrentInfo.img.dir == 0)	//ball on the left
				  {
				  		set_ccr(TURN_PWM, -TURN_PWM);
				  		Open_PID = 3;
				  }
				  else
				  {
				  		set_ccr(-TURN_PWM, TURN_PWM);
				  		Open_PID = 2;
				  }

				  if(TargetCnt++ == 2)
				  {
					  TargetCnt = 0;
					  TargetSubState = 1;
				  }
			  }
			  else
			  {
				  set_ccr(0, 0);
				  Open_PID = 0;
				  if(TargetCnt++ == 3)
				  {
					  TargetCnt = 0;
					  TargetSubState = 0;
				  }
			  }

			  StateCnt = 0;
			  break;

		  case 3:	//State 3
			  Servo_Cam(0);
			  Encoder_PID_init(&EncoderPID_L, CRUISE_PWM, CRUISE_VELOCITY);
			  Encoder_PID_init(&EncoderPID_R, CRUISE_PWM, CRUISE_VELOCITY);
			  set_ccr(CRUISE_PWM, CRUISE_PWM);
			  Open_PID = 1;
			  StateCnt = 0;
			  break;

		  case 4:	//State 4
			  Open_PID = 0;
			  switch(CatchBallSubState)
			  {
			  case 1:
				  set_ccr(0, 0);
//				  if(CatchCnt < 10)
					  Servo_Control_DOWN(1);
					  Servo_Cam(1);
//				  else
				  {
					  CatchBallSubState = 3;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
//			  case 2:
//				  set_ccr(0, 0);
//				  if(CatchCnt < 5)
//
//				  else
//				  {
//					  CatchBallSubState = 3;
//					  CatchCnt = 0;
//					  StateCnt = 0;
//				  }
//				  break;
			  case 3:
				  if(CatchCnt < 18)
				  {
					  Encoder_PID_init(&EncoderPID_L, CRUISE_PWM, CRUISE_VELOCITY);
					  Encoder_PID_init(&EncoderPID_R, CRUISE_PWM, CRUISE_VELOCITY);
					  set_ccr(CRUISE_PWM, CRUISE_PWM);
					  Open_PID = 1;
				  }
				  else
				  {
					  set_ccr(0, 0);
					  Open_PID = 0;
					  CatchBallSubState = 4;
					  CatchCnt = 0;
					  StateCnt = 0;
				  }
				  break;
			  case 4:
				  set_ccr(0, 0);
				  Open_PID = 0;
//				  if(CatchCnt < 5)
					  Servo_Control_DOWN(2);
//				  else
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
		  case 6:	//State 6
			  switch(ReturnSubState)
			  {
			  case 1:
				  if(FindGreenSubState == 0)
				  {
					  if(ReturnCnt == 0)
					  {
						  Encoder_PID_init(&EncoderPID_L, TURN_PWM, TURN_VELOCITY);
						  Encoder_PID_init(&EncoderPID_R, TURN_PWM, TURN_VELOCITY);
					  }
					  set_ccr(-TURN_PWM, TURN_PWM);
					  Open_PID = 2;
					  if(ReturnCnt++ == 3)
					  {
						  ReturnCnt = 0;
						  FindGreenSubState = 1;
					  }
				  }
				  else
				  {
					  set_ccr(0, 0);
					  Open_PID = 0;
					  if(ReturnCnt++ == 3)
					  {
						  ReturnCnt = 0;
						  FindGreenSubState = 0;
					  }
				  }
				  StateCnt = 0;
				  break;

			  case 2:
				  Encoder_PID_init(&EncoderPID_L, CRUISE_PWM, CRUISE_VELOCITY);
				  Encoder_PID_init(&EncoderPID_R, CRUISE_PWM, CRUISE_VELOCITY);
				  set_ccr(CRUISE_PWM, CRUISE_PWM);
				  Open_PID = 1;
				  StateCnt = 0;
				  break;
			  case 3:
				  Brake();
				  Dump();
				  BallCatch = 0;
				  StateCnt = 0;
				  break;
			  }
			  break;
		  case 7:	//State 7
			  Servo_Cam(0);
			  if(ObstacleCnt < 10)
			  {
				  set_ccr(-CRUISE_PWM, -CRUISE_PWM);
				  Open_PID = 0;
			  }
			  else
			  {
				  set_ccr(0, 0);
				  Open_PID = 0;
				  StateCnt = 0;
				  ObstacleCnt = 0;
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
