#include "ds18b20.h"
#include "delay.h"
#include "delay2.h"
#include "sys.h"
#include "usart.h"

u8 DS18B20_DQIN=0;


void DS18B20_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
//    __GPIOC_CLK_ENABLE();            //使能GPIOC时钟
	    GPIO_Initure.Pin=GPIO_PIN_9;	
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;   //推挽输出
//    GPIO_Initure.Pull=GPIO_SPEED_HIGH;           //上拉
//    GPIO_Initure.Speed=GPIO_SPEED_HIGH;      //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

void DS18B20_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
    __GPIOC_CLK_ENABLE();            //使能GPIOC时钟
    GPIO_Initure.Pin=GPIO_PIN_9;		
    GPIO_Initure.Mode=GPIO_MODE_INPUT;   //输入浮空
    GPIO_Initure.Pull=GPIO_PULLUP;           //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;      //GPIO_SPEED_HIGH
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}


void DS18B20_Rst(void)	   
{       
	DS18B20_IO_OUT();   //设置为输出
  DS18B20_DQ_0;  //拉低DQ
 delay_us(750);      //拉低750us
  DS18B20_DQ_1;  //DQ=1 
		delay_us(15);       //15US
}

u8 DS18B20_Check(void) 	   
{
	u8 i;	
	u8 retry=0;
	DS18B20_IO_IN();    //设置为输入
	DS18B20_DQIN=DS18B20_DQ_Read;
  while (DS18B20_DQIN&&retry<200)
	{
		retry++;
		delay_us(1);
		DS18B20_DQIN=DS18B20_DQ_Read;
	};	 
	if(retry>=200)return 1;
	else retry=0;
	DS18B20_DQIN=DS18B20_DQ_Read;
    while (!DS18B20_DQIN&&retry<240)
	{
		retry++;
		delay_us(1);
		DS18B20_DQIN=DS18B20_DQ_Read;
	};
	if(retry>=240)return 1;	    
	return 0;
}

u8 DS18B20_Read_Bit(void) 
{
	u8 t;
  u8 data;
	DS18B20_IO_OUT();   //设置为输出
  DS18B20_DQ_0; 
	delay_us(2);
  DS18B20_DQ_1; 
	DS18B20_IO_IN();    //设置为输入
	delay_us(12);				//设置为12us
//	DS18B20_DQIN=DS18B20_DQ_Read;
	if(DS18B20_DQ_Read==GPIO_PIN_SET)data=1;
  else data=0;
  delay_us(50);         //原始值50  
  return data;
}

u8 DS18B20_Read_Byte(void)   
{        
  u8 i,j,dat;
  dat=0;

	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
  return dat;
}

void DS18B20_Write_Byte(u8 dat)     
 {             
	 u8 t;
    u8 j;
    u8 testb;
    DS18B20_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
          DS18B20_DQ_0;
           delay_us(2);    //原始值是2                        
            DS18B20_DQ_1;
            delay_us(60);             
        }
        else            //写0
        {
            DS18B20_DQ_0;
            delay_us(60);             
            DS18B20_DQ_1;
           delay_us(2);                          
        }
    }
}


void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}

u8 DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_9;           //PC9
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化
 	  DS18B20_Rst();
	  return DS18B20_Check();
}

short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
    short tem;
    DS18B20_Start ();        	   //开始转换
    DS18B20_Rst();
		DS18B20_Check();
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // convert	 
    TL=DS18B20_Read_Byte();     // LSB   
    TH=DS18B20_Read_Byte();     // MSB   
		if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(short)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}
