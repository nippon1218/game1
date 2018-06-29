#include "mlx90614.h"
#include "delay.h"


#define  Nack_counter  10 

u8        DataL;
u8        DataH;
u8 				Pecreg;

void Mlx96014_Init(void)
{
		GPIO_InitTypeDef GPIO_Initure;    
    __GPIOC_CLK_ENABLE();            //使能GPIOC时钟	
	  GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;			//	PC10,PC11，PC10数据，PC11时钟
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull=GPIO_SPEED_HIGH;           //上拉

    GPIO_Initure.Speed=GPIO_SPEED_HIGH;      //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
	
		MLX_SDA_1;
		MLX_SCL_1;
}

void MLX_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
    __GPIOC_CLK_ENABLE();            //使能GPIOC时钟
    GPIO_Initure.Pin=GPIO_PIN_10;		

    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;   //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;           //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;      //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

void MLX_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_Initure;    
    __GPIOC_CLK_ENABLE();            //使能GPIOC时钟
	
    GPIO_Initure.Pin=GPIO_PIN_10;		
    GPIO_Initure.Mode=GPIO_MODE_INPUT;   //输入浮空
    GPIO_Initure.Pull=GPIO_PULLUP;           //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;      //快速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}




//对比单片机的start bit，很类似
void SMBus_StartBit(void)
{
//    MLX_SDA_1;               // Set SDA line 
//    SMBus_Delay(1);      // Wait a few microseconds 
//    MLX_SCL_1;               // Set SCK line  
//    SMBus_Delay(5);      // Generate bus free time between Stop
//    MLX_SDA_0;               // Clear SDA line
//    SMBus_Delay(10);     // Hold time after (Repeated) Start
//                         // Condition. After this period, the first clock is generated.
//                         //(Thd:sta=4.0us min)
//    MLX_SCL_0;               // Clear SCK line
//    SMBus_Delay(2);      // Wait a few microseconds

		MLX_SDA_OUT();
	
    MLX_SDA_1;               // Set SDA line 
    delay_us(5);      // Wait a few microseconds 
    MLX_SCL_1;               // Set SCK line  
    delay_us(5);      // Generate bus free time between Stop
    MLX_SDA_0;               // Clear SDA line
    delay_us(5);     // Hold time after (Repeated) Start
                         // Condition. After this period, the first clock is generated.
                         //(Thd:sta=4.0us min)
    MLX_SCL_0;               // Clear SCK line
    delay_us(5);      // Wait a few microseconds	
	
}


//对比单片机的stop bit，很类似
void SMBus_StopBit(void)
{
			MLX_SDA_OUT();						//SDA输出
	
//    MLX_SCL_0;                // Clear SCK line
//    SMBus_Delay(5);       // Wait a few microseconds
//    MLX_SDA_0;                // Clear SDA line
//    SMBus_Delay(5);       // Wait a few microseconds
//		MLX_SCL_1;                  // Set SCK line
//    SMBus_Delay(10);      // Stop condition setup time(Tsu:sto=4.0us min)
//    MLX_SDA_1;                // Set SDA line
	
	    MLX_SCL_0;                // Clear SCK line
			delay_us(5);       // Wait a few microseconds
			MLX_SDA_0;                // Clear SDA line
			delay_us(5);       // Wait a few microseconds
			MLX_SCL_1;                  // Set SCK line
			delay_us(5);      // Stop condition setup time(Tsu:sto=4.0us min)
			MLX_SDA_1;                // Set SDA line
}

u8 SMBus_SendByte(u8 Tx_buffer)
{
    u8        Bit_counter;
    u8        Ack_bit;
    u8        bit_out;
		u8 				repeat;


		for(repeat=0;repeat<10;repeat++)	
		 {
			for(Bit_counter=8; Bit_counter; Bit_counter--)
			{
					if (Tx_buffer&0x80)
					{
							bit_out=1;       // If the current bit of Tx_buffer is 1 set bit_out
					}
					else
					{
							bit_out=0;      // else clear bit_out
					}
					SMBus_SendBit(bit_out);           // Send the current bit on SDA
					Tx_buffer<<=1;                    // Get next bit for checking
			}
			
			Ack_bit=SMBus_ReceiveBit();           // Get acknowledgment bit		
		
			if(Ack_bit==1)
			{
				SMBus_StopBit();
				//此处不知道需不需要延时？？？？？？？？？？？？？？？？？？？？？？？？
				if(repeat<10)
				{
					SMBus_StartBit();
				}
				else
				{break;}
//				SMBus_StartBit();
			}
			else
			{
				break;
			}
		
		}
    
    return   Ack_bit;
}


void SMBus_SendBit(u8 bit_out)
{
			MLX_SDA_OUT();
//    if(bit_out==0)		//
//    {
//      MLX_SDA_0;   
//    }
//    else
//    {
//			MLX_SDA_1;
//    }
//    SMBus_Delay(2);                            // Tsu:dat = 250ns minimum
//    MLX_SCL_1;                                     // Set SCK line
//    SMBus_Delay(10);                           // High Level of Clock Pulse
//    MLX_SCL_0;                                     // Clear SCK line
//    SMBus_Delay(10);                           // Low Level of Clock Pulse
////        SMBUS_SDA_H();                       // Master release SDA line ,
//    return;
	
	  if(bit_out==0)
    {
      MLX_SDA_0;   
    }
    else
    {
			MLX_SDA_1;
    }
    delay_us(1);                            // Tsu:dat = 250ns minimum
    MLX_SCL_1;                                     // Set SCK line
    delay_us(8);                           // High Level of Clock Pulse
    MLX_SCL_0;                                     // Clear SCK line
    delay_us(8);                           // Low Level of Clock Pulse
//    return;
}

u8 SMBus_ReceiveBit(void)
{
    u8 Ack_bit;

//    MLX_SDA_1;             //引脚靠外部电阻上拉，当作输入
//    MLX_SCL_1;             // Set SCL line
//    SMBus_Delay(2);    // High Level of Clock Pulse
//    if (MLX_READ_SDA==GPIO_PIN_SET)
//    {
//        Ack_bit=1;
//    }
//    else
//    {
//        Ack_bit=0;
//    }
//    MLX_SCL_0;                    // Clear SCL line
//    SMBus_Delay(4);           // Low Level of Clock Pulse
//    return   Ack_bit;
	
		MLX_SDA_OUT();

	  MLX_SDA_1;             //引脚靠外部电阻上拉，当作输入
    MLX_SCL_1;             // Set SCL line

		MLX_SDA_IN();					//此刻是作为输入的
		
    delay_us(8);    // High Level of Clock Pulse
		
    if (MLX_READ_SDA==GPIO_PIN_SET)
    {
        Ack_bit=1;
    }
    else
    {
        Ack_bit=0;
    }
		delay_us(1);		//可以删掉？？
    
		MLX_SCL_0;                    // Clear SCL line
    delay_us(8);           // Low Level of Clock Pulse
    return   Ack_bit;
	
}

u8 SMBus_ReceiveByte(u8 ack_nack)
{
    u8        RX_buffer;
    u8        Bit_Counter;

    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
    {
        if(SMBus_ReceiveBit())         // Get a bit from the SDA line
        {
            RX_buffer <<= 1;           // If the bit is HIGH save 1  in RX_buffer
            RX_buffer |=0x01;
        }
        else
        {
            RX_buffer <<= 1;           // If the bit is LOW save 0 in RX_buffer
            RX_buffer &=0xfe;
        }
    }
    SMBus_SendBit(ack_nack);           // Sends acknowledgment bit
    return RX_buffer;
}


//此函数需要经过测试
void SMBus_Delay(u16 time)
{
    u16 i, j;
    for (i=0; i<1; i++)
    {
        for (j=0; j<time; j++);
    }
}

//void SMBus_Delay(u16 time)
//{
//    u16 i, j;
//    for (i=0; i<time; i++)
//    {
//        ;
//    }
//}

u16 memread(void)
{
	SMBus_StartBit();
	SMBus_SendByte(0x00);	//没弄明白为啥是给定是0xB4

	SMBus_SendByte(0x07);	
	SMBus_StartBit();
	SMBus_SendByte(0x01);

	DataL=SMBus_ReceiveByte(0);
	
	DataH=SMBus_ReceiveByte(0);
	Pecreg=SMBus_ReceiveByte(1);
	SMBus_StopBit();
//	return(DataH*256+DataL);
		return(DataL);
}



u16 SMBus_ReadMemory(u8 slaveAddress, u8 command)
{
    u16 data;               // Data storage (DataH:DataL)
    u8 Pec;                 // PEC byte storage
    u8 DataL=0;             // Low data byte storage
    u8 DataH=0;             // High data byte storage
    u8 arr[6];              // Buffer for the sent bytes
    u8 PecReg;              // Calculated PEC byte storage
    u8 ErrorCounter;        // Defines the number of the attempts for communication with MLX90614


    ErrorCounter=0x00;                                // Initialising of ErrorCounter
        slaveAddress <<= 1;        //2-7位表示从机地址

    do
    {
repeat:
        SMBus_StopBit();                //If slave send NACK stop comunication
        --ErrorCounter;                 //Pre-decrement ErrorCounter
        if(!ErrorCounter)               //ErrorCounter=0?
        {
            break;                      //Yes,go out from do-while{}
        }

        SMBus_StartBit();               //Start condition
        if(SMBus_SendByte(slaveAddress))//Send SlaveAddress 最低位Wr=0表示接下来写命令
        {
            goto  repeat;               //Repeat comunication again
        }
        if(SMBus_SendByte(command))     //Send command
        {
            goto    repeat;             //Repeat comunication again
        }

        SMBus_StartBit();                //Repeated Start condition
        if(SMBus_SendByte(slaveAddress+1))  //Send SlaveAddress 最低位Rd=1表示接下来读数据
        {
            goto        repeat;           //Repeat comunication again
        }

        DataL = SMBus_ReceiveByte(ACK);   //Read low data,master must send ACK
        DataH = SMBus_ReceiveByte(ACK);   //Read high data,master must send ACK
        Pec = SMBus_ReceiveByte(NACK);    //Read PEC byte, master must send NACK
        SMBus_StopBit();                  //Stop condition

        arr[5] = slaveAddress;        
        arr[4] = command;
        arr[3] = slaveAddress+1;         //Load array arr
        arr[2] = DataL;                 
        arr[1] = DataH;                
        arr[0] = 0;                   
        PecReg=PEC_Calculation(arr);     //Calculate CRC
    }
    while(PecReg != Pec);                //If received and calculated CRC are equal go out from do-while{}
        data = (DataH<<8) | DataL;       //data=DataH:DataL
    return data;
}

u8 PEC_Calculation(u8 pec[])
{
    u8         crc[6];
    u8        BitPosition=47;
    u8        shift;
    u8        i;
    u8        j;
    u8        temp;

    do
    {
        /*Load pattern value 0x000000000107*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;
        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;
        /*Set shift position at 0*/
        shift=0;
        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */


        /*Get shift value for pattern value*/
        shift=BitPosition-8;
        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/
        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}

float SMBus_ReadTemp(void)
{   
    return SMBus_ReadMemory(SA, RAM_ACCESS|RAM_TOBJ1)*0.02-273.15;
}









/***************************以下是根据单片机改写*********************************/









