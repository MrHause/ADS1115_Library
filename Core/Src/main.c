/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ADS1115.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//#define ADS1115_ADR (0x48 << 1)
#define ADS1115_ADR 0x48
uint8_t ADSWrite[6];
int16_t readValue;
float voltValue;
const float voltageConv = 6.114 / 32768.0f;

// EXTI Line9 External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*
	if (GPIO_Pin == GPIO_PIN_10) // If The INT Source Is EXTI Line9 (A9 Pin)
	{
		//HAL_Delay(20);
		ADSWrite[0] = 0x00;
		HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADR, ADSWrite, 1, 100);
		HAL_I2C_Master_Receive(&hi2c1, ADS1115_ADR, ADSWrite, 2, 100);
		readValue = ((ADSWrite[0] << 8) | ADSWrite[1]);
		if (readValue < 0)
			readValue = 0;

		voltValue = readValue * voltageConv;
		int a = 2;
	}
	*/
}
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
/*
  ADSWrite[0] = 0x01;
  ADSWrite[1] = 0x40;
  ADSWrite[2] = 0x80;
  HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADR, ADSWrite, 3, 100);

  ADSWrite[0] = 0x03;
  ADSWrite[1] = 0xFF;
  ADSWrite[2] = 0xFF;
  HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADR, ADSWrite, 3, 100);

  ADSWrite[0] = 0x02;
  ADSWrite[1] = 0x00;
  ADSWrite[2] = 0x00;
  HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADR, ADSWrite, 3, 100);
*/
  ADS1115_Config_t configReg;
  configReg.channel = CHANNEL_AIN0_GND;
  configReg.pgaConfig = PGA_6_144;
  configReg.operatingMode = MODE_SINGLE_SHOT;
  configReg.dataRate = DRATE_128;
  configReg.compareMode = COMP_HYSTERESIS;
  configReg.polarityMode = POLARITY_ACTIVE_LOW;
  configReg.latchingMode = LATCHING_NONE;
  configReg.queueComparator = QUEUE_ONE;
  ADS1115_Handle_t *pADS = ADS1115_init(&hi2c1, ADS1115_ADR, configReg);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  readValue = ADS1115_oneShotMeasure(pADS);
	  voltValue = readValue * voltageConv;
	  HAL_Delay(200);


/*
	  ADSWrite[0] = 0x00;
	  HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADR, ADSWrite, 1, 100);
	  HAL_Delay(20);
	  HAL_I2C_Master_Receive(&hi2c1, ADS1115_ADR, ADSWrite, 2, 100);
	  readValue = ((ADSWrite[0] << 8) | ADSWrite[1]);
	  if(readValue < 0)
		  readValue = 0;

	  voltValue = readValue * voltageConv;
	  int a = 2;

	  HAL_Delay(3000);
	  */
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
