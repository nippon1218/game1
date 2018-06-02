#include "bma222.h"

u8 bmadata[3];


void bma222_Init(void)
{
	IIC_Init();			//IIC初始化
}


void bma222_ReadAcc(void)
{
	u8 x_acc=0;
	u8 y_acc=0;	
	u8 z_acc=0;

	IIC_Start(); 
	IIC_Send_Byte(0X30);       //发送写命令  
	IIC_Wait_Ack();  
	IIC_Send_Byte(0x02);    //发送读地址指针  
	IIC_Wait_Ack();             
		
	IIC_Start();             
	IIC_Send_Byte(0X31);           //发送读命令，进入接受模式                
	IIC_Wait_Ack(); 
	
	IIC_Read_Byte(1); 
	bmadata[0]=IIC_Read_Byte(1); 
	
	IIC_Read_Byte(1); 
	bmadata[1]=IIC_Read_Byte(1); 	
	
	IIC_Read_Byte(1); 
	bmadata[2]=IIC_Read_Byte(0); 		
	IIC_Stop();                     //产生一个停止条件 	
	
}











