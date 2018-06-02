
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "sys.h"
#include "delay.h"
#include "myiic.h"
#include "tmp006.h"
#include "bma222.h"

/* USER CODE BEGIN Includes */
#define NB  30
#define CHN  4
//#define CHN  5
#define ADCNB NB*CHN

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define BUFFER_SIZE 256

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
PUTCHAR_PROTOTYPE
{
HAL_UART_Transmit(&huart2 , (uint8_t *)&ch, 1, 0xFFFF);
return ch;
}

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

u16 adcfilter(u16 number,u8 channel);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void uartdamget(void);
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t rx_len=0,bootfirst=1;
uint8_t recv_end_flag=0;
uint16_t rx_buffer[BUFFER_SIZE];

u16 uhADCxConvertedValue[NB][CHN];
unsigned long shijian=0;
volatile int dmaflage=0;
float vcc[CHN]={0};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	u16 m=0;
	u8 i;
	u8 ID_num[12];
	u16 Conf_Read=0;
	u16 Tdie_Temp=0;
	u16 Vobj_Read;  
	float Vobj;  
	float Tdie; 
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

		delay_init(10);
		IIC_Init();
	
	
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
	
  /* USER CODE BEGIN 2 */
//		shijian=HAL_GetTick();
//		printf("\n\r");
//		printf("UID:");
//	  for(i=0;i<12;i++)
//		{
//		ID_num[i] = *(u8 *)(0x1FFF7590+i);
//		printf("%x",ID_num[i]);	
//		}
//		printf("\n\r");
		HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, ADCNB);
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE BEGIN 3 */
//	if(	HAL_GetTick()-shijian>=250)
//	{
//		
//	 HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//	 shijian=HAL_GetTick();	
//	}
//		HAL_Delay(500);
		delay_ms(100);

	 if(dmaflage==1)
	 {
		 dmaflage=0;
		 for(int a=0;a<CHN;a++)
		 {
			 vcc[a]=adcfilter(NB,a)*3.3/4095;
		  printf("AD[%d]= %0.2fV ",a,vcc[a]);
			vcc[a]=0; 
		 }
		  printf("\r");

		 HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, ADCNB);	
	 }
	 
	 		do  
		{  
				// TODO: enter the block content here  
				Conf_Read = tmp006_ReadTwoByte(0x02);  
		} while ((Conf_Read&0x0080) != 0x0080); 
		 Tdie_Temp = tmp006_ReadTwoByte(0x01) ;
		printf("ÎÂ¶È£º%d\r\n",Tdie_Temp);
	 
		bma222_ReadAcc();
		printf("x£º%d\r\n",bmadata[0]);
		printf("y£º%d\r\n",bmadata[1]);
		printf("z£º%d\r\n",bmadata[2]);	 
	 	printf("\r\n");	 
	 
	 
	 uartdamget();
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
//void SystemClock_Config(void)
//{

//  RCC_OscInitTypeDef RCC_OscInitStruct;
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_PeriphCLKInitTypeDef PeriphClkInit;

//    /**Initializes the CPU, AHB and APB busses clocks 
//    */
//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
//  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
//  RCC_OscInitStruct.MSICalibrationValue = 0;
//  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_7;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//    /**Initializes the CPU, AHB and APB busses clocks 
//    */
//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_ADC;
//  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
//  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//    /**Configure the main internal regulator output voltage 
//    */
//  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//    /**Configure the Systick interrupt time 
//    */
//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//    /**Configure the Systick 
//    */
//  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

//  /* SysTick_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	printf("\r");
	dmaflage=1;
	HAL_ADC_Stop_DMA(&hadc1);
}

u16 adcfilter(u16 number,u8 channel)
{
	u8 i=0;
	u32 tempe=0;
	u32 sum=0;
	for(i=0;i<number;i++)
	{
		tempe=uhADCxConvertedValue[i][channel];
		sum+=tempe;
	}
	return (u32)(sum/number);
}
void uartdamget(void)
{
	if(recv_end_flag ==1)
	{

		printf("\nrx_len=%d\r\n",rx_len);

		for(int i=0;i<rx_len;i++)
		{
				printf("%c",rx_buffer[i]);
		}
		printf("\r\n");	  
		for(int i = 0; i < rx_len ; i++) rx_buffer[i]=0;
		rx_len=0;
		recv_end_flag=0;
	}
	HAL_UART_Receive_DMA(&huart2,(uint8_t*)rx_buffer,BUFFER_SIZE);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
