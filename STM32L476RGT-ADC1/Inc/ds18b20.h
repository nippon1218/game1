#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"

#define DS18B20_DQ_1 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define DS18B20_DQ_0 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)

#define DS18B20_DQ_Read	 HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9)	


u8 DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20 
void DS18B20_IO_OUT(void);
void DS18B20_IO_IN(void);
#endif
