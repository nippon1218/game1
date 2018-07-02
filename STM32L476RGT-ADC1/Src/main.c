
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
//#include "delay2.h"
#include "myiic.h"
#include "tmp006.h"
#include "bma222.h"
#include "esp8266.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "ds18b20.h"
//#include "mlx90614.h"


#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

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


u16 adcfilter(u16 number,u8 channel);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

uint8_t rx_len=0,bootfirst=1;
uint8_t recv_end_flag=0;
uint16_t rx_buffer[BUFFER_SIZE];

u16 uhADCxConvertedValue[NB][CHN];

volatile int dmaflage=0;
//float vcc[CHN]={0};
double vcc[CHN]={0};
double ds18b20_temp;


int main(void)
{
	u8 flag;
	u8 t=0,report=1;			//默认开启上报？？？
	u16 m=0;
	u8 i;
	u8 ID_num[12];
	u16 Conf_Read=0;
	u16 Tdie_Temp=0;
	u16 Vobj_Read;  
	float test=0;
	float Vobj;  
	float Tdie; 
	float pitch,roll,yaw; 		//欧拉角
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temp;					//温度
	short temperature;  
	
  HAL_Init();
  SystemClock_Config();
	delay_init(80);
	IIC_Init();
//	Mlx96014_Init();
	
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	

	
	while(MPU1_Init())
	{
		u2_printf("MPU_Init()\r\n");
		delay_us(1000000);
	}					//初始化MPU6050
	
	
//	while(mpu_dmp_init())
//	{
//		u2_printf("mpu_dmp_init()\r\n");
//		delay_us(1000000);
//	}
	
//	ESP8266_AP_Init(4);
	
	ESP8266_STAClient_Init1(4);
	
	while(DS18B20_Init())	
	{
		u2_printf("no ds18b20\r\n");
		delay_ms(500);
	}
	
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, ADCNB);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); 

	
  while (1)
  {
		delay_us(500000);

		if(dmaflage==1)
		{
		 dmaflage=0;
		 for(int a=0;a<CHN;a++)
		 {
			 vcc[a]=adcfilter(NB,a)*3.3/4095;
			 test=vcc[a];
//				u2_printf("AD[%d]= %0.2fV ",a,vcc[a]);
//			vcc[a]=0; 
		 }
			u2_printf("\r\n");

		  Sort(4);	//对四个参数进行排序
		 
//		 u2_printf("最小值:%0.2f,最大值:%0.2f\r\n",vcc[0],vcc[3]);
		
		 
		 
//		 u3_printf("YL1*%0.2f*%0.2f*%0.2f%*%0.2f*",test,test+2,test,31);
		 HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&uhADCxConvertedValue, ADCNB);	
		}
//		do  
//		{   
//			Conf_Read = tmp006_ReadTwoByte(0x02);  
//		} while ((Conf_Read&0x0080) != 0x0080); 
//		Tdie_Temp = tmp006_ReadTwoByte(0x01) ;
//		
//		u2_printf("temp：%d\r\n",Tdie_Temp/125);
////		u3_printf("WD1*%d*%d*%d%*%d*\r\n",Tdie_Temp/125,Tdie_Temp/125,Tdie_Temp/125,Tdie_Temp/125);		
//		delay_us(800000);
//		bma222_ReadAcc();
//		u3_printf("ZT1*x:%d, y:%d, z:%d*\r\n",bmadata[0]/10,bmadata[1]/10,bmadata[2]/10);	
//		u2_printf("x：%d\r\n",bmadata[0]);
//		u2_printf("y：%d\r\n",bmadata[1]);
//		u2_printf("z：%d\r\n",bmadata[2]);	 
//		u2_printf("\r\n");  
//		uartdamget();
//			temperature=SMBus_ReadTemp();			//读取温度
//			temperature=memread();

		temperature=DS18B20_Get_Temp();		//获取温度
//		u2_printf("测出的温度:%d.%d℃\r\n",temperature/10,temperature%10);
		
		ds18b20_temp=(double)(temperature/10+0.1*(temperature%10));
		u2_printf("测出的温度:%0.1f℃\r\n",ds18b20_temp);
		
		temp=MPU_Get_Temperature();	//得到温度值
		u2_printf("tem:%d",temp);
		
		flag=MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
//		u2_printf("MPU_Get_Accelerometer=%d",flag);
		u2_printf("aacx=%d,aacy=%d,aacz=%d\r\n",aacx,aacy,aacz);
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);				//得到陀螺仪数据
//		u2_printf("MPU_Get_Gyroscope=%d",flag);
		u2_printf("gyrox=%d,gyroy=%d,gyroz=%d\r\n",aacx,aacy,aacz);		

		u3_printf("BL1*%0.1f*X:%d Y:%d Z:%d*%0.2f\r\n",ds18b20_temp,gyrox,gyroy,gyroz,5*(3.3-vcc[0]));

//		flag=mpu_dmp_get_data(&pitch,&roll,&yaw);
//		u2_printf("flag=%d\r\n",flag);



//		if(flag==0)
//	{
//		u2_printf("success***************************\r\n");
//		temp=MPU_Get_Temperature();	//得到温度值
//		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
//		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据

//		
//		if((t%10)==0)
//		{ 
//			if(temp<0)
//			{
//				u2_printf("-");
//				temp=-temp;		//转为正数
//			}
//			u2_printf("%d",temp/100);
//			u2_printf("%d",temp%100);
//			u2_printf("%d",temp%10);
//			temp=pitch*10;
//			if(temp<0)
//			{
//				u2_printf("-");
//				
//				temp=-temp;		//转为正数
//			}
//			u2_printf("%d",temp/10);
//			u2_printf("%d",temp%10);

//			temp=roll*10;
//			if(temp<0)
//			{
//				u2_printf("-");
//				temp=-temp;		//转为正数
//			}
//			u2_printf("%d",temp/10);
//			u2_printf("%d",temp%10);

//			temp=yaw*10;
//			if(temp<0)
//			{
//				u2_printf("-");
//				temp=-temp;		//转为正数
//			}
//			u2_printf("%d",temp/10);
//			u2_printf("%d",temp%10);
//			t=0;
//		}
//		
//	}
//	else
//	{
//		u2_printf("hellojapan,t=%d**********\r\n",t);
//	}

//		t++;
//		if(t>14)
//		{
//			t=0;
//			 u3_printf("YL1*%d*%d*%d%*%d*",10.7,test,20,30);
//		}
		
	}
}


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


