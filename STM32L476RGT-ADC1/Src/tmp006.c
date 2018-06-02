#include "tmp006.h"


u8 tmp006_Init(void)
{	
	 IIC_Init();			//IIC初始化 	
}

u16 tmp006_ReadTwoByte(u8 daddr)
{
	u16 temp;  
	IIC_Start();    
	IIC_Send_Byte(0X82);       //发送写命令  
	IIC_Wait_Ack();  
	IIC_Send_Byte(daddr);    //发送读地址指针  
	IIC_Wait_Ack();             
		
	IIC_Start();             
	IIC_Send_Byte(0X83);           //发送读命令，进入接受模式                
	IIC_Wait_Ack();    
	temp = IIC_Read_Byte(1);    //读高八位数据  
	temp = temp<<8;  
	temp += IIC_Read_Byte(0);   //读低八位数据  
	IIC_Stop();                     //产生一个停止条件  
	return temp; 	
}









