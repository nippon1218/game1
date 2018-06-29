#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"

#define DS18B20_DQ_1 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define DS18B20_DQ_0 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)

#define DS18B20_DQ_Read	 HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9)	


u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20 
void DS18B20_IO_OUT(void);
void DS18B20_IO_IN(void);
#endif
