#include "esp8266.h"
#include "usart.h"
#include "timer.h"


u8 wifi_ssid[20]="Medical_Bed";	            //热点名称
u8 wifi_ssid_password[20]="12345678";	    //热点密码  

//WiFi配置-路由-默认值
u8 wifi_station[20]="KFHL";					//路由器名称
u8 wifi_station_password[20]="12345678";	//路由器密码
u8 wifi_ap_ip[15]="192.168.1.105";          //IP地址
u16 wifi_ap_port=8086;                      //端口号

/***********************************************************************
 函数名      ：atk_8266_at_response(u8 mode)  
 函数功能    ：用串口2打印WiFi接收到的数据。
 输入        ：0：USART3_RX_LEN不清零，1：清零
 输出        ：无  
************************************************************************/
void atk_8266_at_response(u8 mode)
{
	if(USART3_RX_LEN&0X8000)			//等待返回值
	{ 
		USART3_RX_BUF[USART3_RX_LEN&0X7FFF]=0;	//串口4接收数组末尾添0
		u2_printf("%s",USART3_RX_BUF);					//串口2输出串口4接收数组内容
		if(mode)USART3_RX_LEN=0;								//0：USART3_RX_LEN不清零，1：清零
	} 
}


/***********************************************************************
 函数名      ：atk_8266_check_cmd(u8 *str)  
 函数功能    ：判断USART3_RX_BUF数组中有无指定字符串
 输入        ：0：USART3_RX_LEN不清零，     1：清零
 输出        ：指定字符串第一个字符的地址
************************************************************************/
u8* atk_8266_check_cmd(u8 *str)
{	
	char *strx=0;
	if(USART3_RX_LEN&0X8000)	//等待返回值
	{ 
		USART3_RX_BUF[USART3_RX_LEN&0X7FFF]=0;	//串口4接收数组末尾添0
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str); 		//获取输入字符串str第一个字符的地址
	} 
	return (u8*)strx;					//反回字符串首地址
}

/***********************************************************************
 函数名      ：atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
 函数功能    ：发送WiFi指令
 输入        ：cmd:指令,ack:返回值,waittime:超时时间
 输出        ：1: 指令发送成功，返回值正确  0：失败  

************************************************************************/
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u16 time=0;
	u8 res=0; 
	USART3_RX_LEN=0;												//清空串口4状态标记位
	u3_printf("%s\r\n",cmd);              //发送CMD指令
	time = 0;
	while(time<waittime)									//最大重复运行次数
	{
		delay_ms(40);		
		if(atk_8266_check_cmd(ack))       //判断返回值
		{
			break;
		}	
		time++;				
	}
	if(time<waittime)res=1;                   //如果在超时时间内break出来，则res=1

//	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);	

	return res;
} 

/***********************************************************************
 函数名      ：atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
 函数功能    ：发送数据
 输入        ：
 输出        ：

************************************************************************/
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_LEN=0;				//清空串口4状态标记位
	u3_printf("%s",data);	//发送数据data
	if(ack&&waittime)			//保证ack和等待时间非空
	{
		while(--waittime)	
		{
			delay_ms(5);			//延时
			if(USART3_RX_LEN&0X8000)			//等待返回值ack
			{
				if(atk_8266_check_cmd(ack))break;			//判断模块返回值
				USART3_RX_LEN=0;						//清空串口4状态标志位
			} 
		}
		if(waittime==0)res=1; 		//若waittime为0，返回1
	}
	return res;
}

/***********************************************************************
 函数名      ：atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
 函数功能    ：发送数据给设备0
 输入        ：字符串
 输出        ：无 

************************************************************************/
void esp8266_send_data(u8 *data)
{
	u16 i;
	i=strlen((const char*)data);          //获取数据长度
	u3_printf("AT+CIPSEND=0,%d\r\n",i);   //发送AT+CIPSEND指令
	delay_ms(5);       		//延时                  
	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);		//清除串口4接收缓存区	
	USART3_RX_LEN=0;                           	//清空串口4状态标志位    
	u3_printf("%s",data);                         //发送数据字符数据data
}

/***********************************************************************
 函数名      ：void ESP8266_send_data(u8 id,u8 *data)
 函数功能    ：发送数据给设备号为id的设备
 输入        ：id：设备号；data：要发送的数据
 输出        ：无 

************************************************************************/
void ESP8266_send_data(u8 id,u8 *data)
{
	u16 i;
	i=strlen((const char*)data);                //获取数据长度
	u3_printf("AT+CIPSEND=%d,%d\r\n",id,i);     //发送AT+CIPSEND指令
	delay_ms(5);                     		//延时    
	memset(USART3_RX_BUF,0,50);	                //清除串口4接收缓存区	
	USART3_RX_LEN=0;                            	//清空串口4状态标志位   
	u3_printf("%s",data);                       //发送数据字符数据data	
	delay_ms(200);
	memset(USART3_RX_BUF,0,50);	                //清除串口4接收缓存区	
	USART3_RX_LEN=0; 														//清空串口4状态标志位
}

/***********************************************************************
 函数名      ：atk_8266_quit_trans(void)
 函数功能    ：退出透传
 输入        ：无
 输出        ：1:成功 0:失败

************************************************************************/
u8 atk_8266_quit_trans(void)
{
	while((UART4->ISR&0X40)==0);   //循环发送"+",3次
	UART4->TDR='+';      
	delay_ms(15);				
	while((UART4->ISR&0X40)==0);	
	UART4->TDR='+';      
	delay_ms(15);	
	while((UART4->ISR&0X40)==0);	
	UART4->TDR='+';      
	delay_ms(500);	
	return atk_8266_send_cmd("AT","OK",20);			
}


/***********************************************************************
 函数名      ：atk_8266_get_wanip(u8* ipbuf)
 函数功能    ：获取ipbuf中，介于"\"和"\"中的数据
 输入        ：无
 输出        ：无

************************************************************************/
void atk_8266_get_wanip(u8* ipbuf)
{
	  u8 *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))	//发送AT+CIFSR指令
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");		//获取第1个"\"的地址
		p1=(u8*)strstr((const char*)(p+1),"\"");	//获取第1个"\"以后，第二个"\"
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);			//写入字符串
}

/***********************************************************************
 函数名      ：esp8266_get_ip(u8* ipbuf)
 函数功能    ：从ipbuf中获取路由器分配给STM的IP地址，介于"\"和"\"中的数据
 输入        ：ipbuf：获取IP的数组
 输出        ：无

************************************************************************/
void esp8266_get_ip(u8* ipbuf)
{
	  u8 *p,*p1;	
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))//发送AT+CIFSR指令
		{
			ipbuf[0]=0;
			return;
		}		
		p=atk_8266_check_cmd("\"");								//获取第1个"\"的地址
		p1=(u8*)strstr((const char*)(p+1),"\"");	//获取第1个"\"以后，第二个"\"
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);		//写入字符串
		u2_printf("\r\n%s\r\n",ipbuf);		//串口2发送
}

/***********************************************************************
 函数名      ：atk_8266_SET_AP(u8 *ssid,u8 *password,u8 mode,u16 timeout)
 函数功能    ：esp8266设置AP模式
 输入        ：ssid,password,模式，超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 atk_8266_SET_AP(u8 *ssid,u8 *password,u8 mode,u16 timeout)
{
	u8 res=0;
	u16 time =0;	
	USART3_RX_LEN=0;	//清空串口4状态标志位
	u3_printf("AT+CWSAP=\"%s\",\"%s\",4,%d\r\n",ssid,password,mode);  //发送指令
	time=0;
	while(time<timeout)			//最大重复运行次数
	{
		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
		break;
		delay_ms(10);
		time++;
	}
	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;
}

/***********************************************************************
 函数名      ：atk_8266_SET_STA(u8 *ssid,u8 *password,u16 timeout)
 函数功能    ：esp8266设置STA模式
 输入        ：ssid,password,模式，超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 atk_8266_SET_STA(u8 *ssid,u8 *password,u16 timeout)
{	
	 u8 res=0;
	 u16 time =0;
	 USART3_RX_LEN=0;	//清空串口4状态标志位
	u3_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,password);  //发送指令
//	u3_printf("AT+CWJAP=\"BLMQ\",\"12345678\"\r\n");
	time=0;
	while(time<timeout)			//最大重复运行次数
	{
		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
	    break;
	    delay_ms(40);
		time++;
	}
	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;	 
}


u8 Esp8266_JoinIp(u16 timeout)
{	
//	 u8 res=0;
//	 u16 time =0;
//	 USART3_RX_LEN=0;	//清空串口4状态标志位
////	u3_printf("AT+CIPSTART=\"%s\",\"%s\",%s\r\n",tcp,ip,Port);  //发送指令
//	u3_printf("AT+CIPSTART=\"TCP\",\"192.168.1.105\",8086\r\n");  //发送指令
//	time=0;
//	while(time<timeout)			//最大重复运行次数
//	{
//		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
//	    break;
//	    delay_ms(40);
//		time++;
//	}
//	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
//	return res;	 
	  u8 res = 0;
		u16 time =0;
		USART3_RX_LEN=0;
//  
	delay_ms(2000);
  if(atk_8266_send_cmd("AT+CIPMUX=0","OK",80))
	{
	   u2_printf("单连接成功\r\n");	
	}
	else
	{
	  u2_printf("单连接失败\r\n");	
	}
	delay_ms(2000);
  if(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.105\",8086","OK",250))
	{
	  delay_ms(3000); 
		u2_printf("ip设置成功\r\n");
//		atk_8266_at_response(1);
	}
	else
	{
	  u2_printf("端口设置失败"); 	
	}
	time=0;
   while(time<timeout)
	 {
	    if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))
	    break;
	    delay_ms(70);
			time++;
	 }
	 if(time<timeout) res =1;

	 return res;
	
	
}


/***********************************************************************
 函数名      ：atk_8266_SET_IPPORT(u16 timeout)
 函数功能    ：esp8266设置多路连接，并设置IP端口为192.168.1.115
 输入        ：超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 atk_8266_SET_IPPORT(u16 timeout)
{
	u16 time =0;
	u8 res = 0;
	USART3_RX_LEN=0;	//清空串口4状态标志位

	if(atk_8266_send_cmd("AT+CIPMUX=1","OK",50))		//发送AT+CIPMUX=1指令
	{
		u2_printf("\r\n开启多路连接成功\r\n");		
	}
	else
	{
		u2_printf("\r\n开启多路连接失败\r\n");			
	}
		
	if(atk_8266_send_cmd("AT+CIPAP=\"192.168.1.115\"","OK",100))   //配置IP地址：192.168.1.115
	{
		u2_printf("设置IP成功");	
	}
	else
	{
		u2_printf("设置IP失败");	
	}
	
	u3_printf("AT+CIPSERVER=1,8086\r\n");    //配置端口号为8086
	time=0;
	while(time<timeout)			//最大重复运行次数
	{
		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
		break;
		delay_ms(10);
		time++;
	}
	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;
}

/***********************************************************************
 函数名      ：atk_8266_SET_IPPORT(u16 timeout)
 函数功能    ：设置端口号
 输入        ：超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 atk_8266_SET_PORT(u16 timeout)
{
	u8 res = 0;
	USART3_RX_LEN=0;	//清空串口4状态标志位

//  if(atk_8266_send_cmd("AT+CIPMUX=1","OK",450))
//	{
//		 u2_printf("\r\n开启多路连接成功\r\n");
//	}
//	  else
//	{
//	  u2_printf("\r\n开启多路连接失败\r\n");		
//	}
		
	if(atk_8266_send_cmd("AT+CIPSERVER=1,8080","OK",50))//发送"AT+CIPSERVER=1,8080"指令
	{
		delay_ms(1000); u2_printf("\r\n端口设置成功\r\n");res=1;
	}
	else
	{
		u2_printf("\r\n端口设置失败\r\n");	res=0;
	}
//	time=0;
//   while(time<timeout)			//最大重复运行次数
//	 {
//	    if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
//	    break;
//	    delay_ms(20);
//			time++;
//	 }
//	 if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;
}


/***********************************************************************
 函数名      ：ESP8266_SET_IP(u8 *Ip,u16 timeout)
 函数功能    ：设置端口号
 输入        ：超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 ESP8266_SET_IP(u8 *Ip,u16 timeout)
{
	u8 res = 0;
	u16 time =0;
	USART3_RX_LEN=0;	//清空串口4状态标志位

	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);	//清除串口4接收缓存区	
	
  u3_printf("AT+CIPAP=\"%s\"\r\n",Ip);  //发送指令	
	delay_ms(100);
	time=0;
	while(time<timeout)			//最大重复运行次数
	{
		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
	    break;
	    delay_ms(40);
		time++;
	}
	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;	 
}

/***********************************************************************
 函数名      ：ESP8266_SET_PORT(u8 *Ip,u16 timeout)
 函数功能    ：设置端口号
 输入        ：超时时间
 输出        ：0：失败  1：成功

************************************************************************/
u8 ESP8266_SET_PORT(u16 Port,u16 timeout)
{
	u8 res = 0;
	u16 time =0;	
	USART3_RX_LEN=0;	//清空串口4状态标志位

	memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);	//清除串口4接收缓存区	
	
  u3_printf("AT+CIPSERVER=1,%d\r\n",Port);  //发送指令	
	time=0;
	while(time<timeout)			//最大重复运行次数
	{
		if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))		//如果返回值为"OK"
	    break;
	    delay_ms(40);
		time++;
	}
	if(time<timeout) res =1;		//若当前运行次数小于最大次数，返回1，否则返回0
	return res;	 
}


/***********************************************************************
 函数名      ：esp8266_Get_IP(u16 timeout)
 函数功能    ：
 输入        ：无
 输出        ：无

************************************************************************/
void esp8266_Get_IP(void)
{
	if(atk_8266_send_cmd("AT+CIFSR","OK",500))//发送AT+CIFSR指令
		{u2_printf("请确定返回IP地址");}
	else
		{u2_printf("获取返回值失败");}
	atk_8266_at_response(1);	//发送串口模块返回值
}



/***********************************************************************
 函数名：ESP8266_AP_Init
 功能：  初始化esp8266的AP模式
 参数：  无
 返回值：无
 其他: 热点名称ATK_ESP8266;密码：12345678;端口号：8086

***********************************************************************/
void ESP8266_AP_Init(u8 n)
{
	 u8 i;
	 for(i=0;i<n;i++)	  //最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+CWMODE=2","OK",60))	//发送指令：AT+CWMODE=2
		 {
			u2_printf("\r\n模式设置成功\r\n");
			break;
		 }
		 else
		 {
			u2_printf("\r\n第%d次模式设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
		delay_ms(200);			 
	 }

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+CIPMUX=1","OK",60))	//发送指令：AT+CIPMUX=1
		 {
			u2_printf("\r\n多连接开启\r\n");
			break;
		 }
		 else
		 {	u2_printf("\r\n第%d次多连接开启失败，准备继续尝试设置\r\n",(i+1)); }
		 delay_ms(200);			 
	 }	 

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(atk_8266_SET_AP(wifi_ssid,wifi_ssid_password,WPA_WPA2_PSK,500)) //设置网络名称，密码，加密方式
			{
				delay_ms(300);	
				u2_printf("\r\nAP设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次AP设置尝试失败，准备继续尝试设置\r\n",i+1);
			}
			delay_ms(300);	
	 }	 

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(ESP8266_SET_IP(wifi_ap_ip,60))			//设置IP
			{
				u2_printf("\r\nIP设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次IP设置尝试失败，准备继续尝试设置\r\n",i+1);
			}		
			delay_ms(400);			
	 }

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {		 
			if(ESP8266_SET_PORT(wifi_ap_port,50))		//设置端口号
			{
				u2_printf("\r\n端口设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次端口设置尝试失败，准备继续尝试设置\r\n",i+1);
			}
			delay_ms(1000);					
	 }
	 	 	for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {		 
		 if(atk_8266_send_cmd("AT+CIPSTO=150","OK",60))	//发送指令：AT+CWMODE=2
		 {
			u2_printf("\r\n超时时间设置成功\r\n");
			break;
		 }
		 else
		 {
			u2_printf("\r\n第%d次超时时间设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
		delay_ms(200);				
	 }
}

//STA Client初始化
void ESP8266_STAClient_Init(u8 n)
{
	 u8 i;
	 for(i=0;i<n;i++)	  //最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+CWMODE=1","OK",60))	//发送指令：AT+CWMODE=2
		 {
			u2_printf("\r\n模式设置成功\r\n");
			break;
		 }
		 else
		 {
			u2_printf("\r\n第%d次模式设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
		delay_ms(200);			 
	 }
	delay_ms(1000);

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_SET_STA(WIFI_STATION,PASSWORD,600))
		 {
			u2_printf("\r\nwifi连接成功\r\n");
			break;
		 }
		 else
		 {	u2_printf("\r\n第%d次wifi连接失败，准备继续尝试设置\r\n",(i+1)); }
		 delay_ms(700);			 
	 }		 
	 delay_ms(3000);
	 
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(Esp8266_JoinIp(70))
		 {
			u2_printf("\r\n加入Ip成功\r\n");
			break;
		 }
		 else
		 {	u2_printf("\r\n第%d次加入Ip失败，准备继续尝试设置\r\n",(i+1)); }
		 delay_ms(1000);			 
	 }		 
	 delay_ms(2000);
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(atk_8266_send_cmd("AT+CIPMODE=1","OK",550))
			{
				 u2_printf("\r\nMODE设置成功\r\n");
				 break;
			}
				else
			{
				u2_printf("\r\n第%d次MODE设置失败，准备继续尝试设置\r\n",i+1);		
			}
			delay_ms(600);
	 }	 

	 delay_ms(1000);


	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		if(atk_8266_send_cmd("AT+CIPSEND","OK",120))        //发送开始传输指令"AT+CIPSEND"，并检测返回值是否为"OK"
		  {u2_printf("\r\n开始传输成功\r\n");break;}
		else
		  {u2_printf("\r\n开始传输失败\r\n");}		
			delay_ms(400);			
	 }
	 delay_ms(1000);
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {		 
		 if(atk_8266_send_cmd("AT+CIPSTO=150","OK",60))	//发送指令：AT+CWMODE=2
		 {
			u2_printf("\r\n超时时间设置成功\r\n");
			break;
		 }
		 else
		 {
			u2_printf("\r\n第%d次超时时间设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
		delay_ms(200);				
	 }
	 	u3_printf("YL1*20*14*11*8*"); 
}

/***********************************************************************
 函数名    ：esp8266_STA_SERVER_Init
 功能      ：初始化esp8266的STA模式
 参数      ：无
 返回值    ：无
 其他: 

***********************************************************************/
u8 atk_8266_SET_TCPIP(u16 timeout)
{
  u8 res = 0;
	u16 time =0;
	USART3_RX_LEN=0;
//  
	delay_ms(1200);
  if(atk_8266_send_cmd("AT+CIPMUX=0","OK",80))
	{
	   u2_printf("开启单连接成功");	
	}
	  else
	{
	  u2_printf("开启单连接失败");	
	}
	delay_ms(1200);
  if(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.103\",8086","OK",100))
	{
	  delay_ms(3000); u2_printf("端口设置成功\r\n");	atk_8266_at_response(1);
	}
	  else
	{
	  u2_printf("端口设置失败"); 	
	}
	time=0;
   while(time<timeout)
	 {
	    if(strstr((const char *)USART3_RX_BUF,(const char *)"OK"))
	    break;
	    delay_ms(70);
			time++;
	 }
	 if(time<timeout) res =1;

	 return res;
}


void ESP8266_STAClient_Init1(u8 n)
{
  u8 i,j;                                            //定义i，j为u8类型变量
	u8 repeat=3;                                       //重复连接网络次数
	delay_ms(200);
	for(i=0;i<1;i++)
	{
    //设置STA-AP模式1	
		for(j=0;j<repeat;j++)
		{
			if(atk_8266_send_cmd("AT+CWMODE=1","OK",70))   //发送指令"AT+CWMODE=1"，并检测返回值是否为“OK”
		  {
				u2_printf("\r\nSTA-AP模式设置成功\r\n");
				break;
			}    
			else
		  {u2_printf("\r\nSTA-AP模式设置失败\r\n");}		
		}
		delay_ms(200);
		for(j=0;j<repeat;j++)
		{
			if(atk_8266_SET_STA(WIFI_STATION,PASSWORD,900)) //发送指令加入网络，要加入网络的名称和密码为WIFI_STATION,PASSWORD
			{
				 delay_ms(2000);
				 u2_printf("\r\n加入AP模式设置完成\r\n");
				break;
			}
			else
			{u2_printf("\r\n加入AP模式设置失败r\n");}			
		}
		delay_ms(700);
		
		for(j=0;j<repeat;j++)
		{
			if(atk_8266_SET_TCPIP(50))                      //连接指定主机的IP地址
      {u2_printf("\r\n连接指定主机的IP地址成功\r\n");     
			break;
			}    
			else
		  {u2_printf("\r\n连接指定主机的IP地址失败\r\n");delay_ms(500);}	
		}	
		delay_ms(700);		
    //开启传输模式	
			for(j=0;j<repeat;j++)
			{	
			if(atk_8266_send_cmd("AT+CIPMODE=1","OK",500))   //发送开启传输模式指令"AT+CIPMODE=1"，并检测返回值是否为“OK”。
				{u2_printf("\r\n开启传输模式成功\r\n");break;}   
			else
				{u2_printf("\r\n开启传输模式失败\r\n");}	
			}	
		//开始传输
		delay_ms(1000);	
		for(j=0;j<repeat;j++)
		{
		if(atk_8266_send_cmd("AT+CIPSEND","OK",70))        //发送开始传输指令"AT+CIPSEND"，并检测返回值是否为"OK"
		  {u2_printf("\r\n开始传输成功\r\n");break;}
		else
		  {u2_printf("\r\n开始传输失败\r\n");}	
		}
		delay_ms(1000);	
		//设置服务器超时时间
//		for(j=0;j<repeat;j++)
//		{
//			if(atk_8266_send_cmd("AT+CIPSTO=500","OK",70))   //发送设置超时时间指令"AT+CIPSTO=500"，并检测返回值是否为“OK”
//				{u2_printf("\r\n连接正常\r\n");break;}
//			else
//				{u2_printf("\r\n连接超时\r\n");}	
//		}
		//u3_printf("YL1*20*14*11*8*"); 
		//u3_printf("连接成功");                             //发送“连接成功”标识至上位机界面
	}
}


void esp8266_STA_SERVER_Init(void)
{
  u8 i;
	for(i=0;i<2;i++)
	{
		delay_ms(100);		
		if(atk_8266_send_cmd("AT+CWMODE=1","OK",500))		//发送指令：AT+CWMODE=1
			{u2_printf("\r\n模式设置成功\r\n");}
		else
			{u2_printf("\r\n模式设置失败\r\n");}	
			
		if(atk_8266_send_cmd("AT+RST","OK",3000))		//发送指令：AT+RST
			{u2_printf("\r\n重置成功\r\n");}
		else
			{u2_printf("\r\n重置失败\r\n");}
			
		delay_ms(5000);
					
		if(atk_8266_SET_STA(WIFI_STATION,PASSWORD,600)) 	//给定路由器名称和密码
			{delay_ms(5000);u2_printf("\r\nWiFi连接成功\r\n");}
		else
			{u2_printf("\r\nWiFi连接失败\r\n");}
			
		delay_ms(3000);

		if(atk_8266_SET_PORT(600)) 								//设置网络端口号
			{ u2_printf("\r\n 端口设置成功 \r\n"); }		  		
		else	
			{ u2_printf("\r\n 端口设置成功 \r\n"); }		
	}
}

/***********************************************************************
 函数名    ：esp_8266_apsta_Init
 功能      ：初始化esp8266的AP服务器-TCP服务器模式
 参数      ：n,失败重连次数
 返回值    ：无
 其他: 

***********************************************************************/
void esp_8266_apsta_Init(u8 n)
{
   u8 i;
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+CWMODE=3","OK",100))	//发送指令：AT+CWMODE=3
		 {
			u2_printf("\r\n模式设置成功\r\n");
			break;
		 }
		 else
		 {
			u2_printf("\r\n第%d次模式设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
	 }
	for(i=0;i<n;i++)		//最大循环次数设置为参数n
	{
		if(atk_8266_send_cmd("AT+RST","OK",100))	//发送指令：AT+RST
		{
			u2_printf("\r\n重启设置成功\r\n");
			delay_ms(2000);	
			break;
		}
		else
		{
			u2_printf("\r\n第%d次重启尝试失败，准备继续重启设置\r\n",i+1);
		}
//			 delay_ms(1000);		 
	}
	for(i=0;i<n;i++)		//最大循环次数设置为参数n
	{
		if(atk_8266_SET_AP(wifi_ssid,wifi_ssid_password,WPA_WPA2_PSK,500))		//设置网络名称，密码，加密方式
		{
			u2_printf("\r\nAP设置成功\r\n");
			break;
		}
		else
		{
			u2_printf("\r\n第%d次AP设置尝试失败，准备继续尝试设置\r\n",i+1);
		}
			delay_ms(100);
	 }

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(ESP8266_SET_IP(wifi_ap_ip,60))							//设置网络IP地址
			{
				u2_printf("\r\nIP设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次IP设置尝试失败，准备继续尝试设置\r\n",i+1);
			}		
			delay_ms(500);			
	 }

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {		 
			if(ESP8266_SET_PORT(wifi_ap_port,50))			//设置网络端口号
			{
				u2_printf("\r\n端口设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次端口设置尝试失败，准备继续尝试设置\r\n",i+1);
			}
			delay_ms(1000);					
	 }

	
	for(i=0;i<n;i++)		//最大循环次数设置为参数n
	{
		if(atk_8266_SET_STA(wifi_station,wifi_station_password,700))			//给定路由器名称和密码
		{
			delay_ms(4000);
			u2_printf("\r\nWIFI-STATION设置成功\r\n");
			break;
		}
		else
		{
			u2_printf("\r\n第%d次WIFI-STATION设置尝试失败，准备继续尝试设置\r\n",i+1);
		}
	 }
	
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
//	   delay_ms(50);
		if(atk_8266_SET_PORT(200))					//设置端口号
		{
			u2_printf("\r\n服务器端口设置成功\r\n");
			break;
		}
		else
		{
			u2_printf("\r\n第%d次服务器端口设置失败，准备继续尝试设置\r\n",i+1);
		}
	 }	 
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		delay_ms(20);
		if(atk_8266_send_cmd("AT+CIPSTO=2200","OK",500))			//AT+CIPSTO=2200，配置断开连接时长
		{
			u2_printf("\r\n超时时间设置成功\r\n");
			break;
		}
		else
		{
			u2_printf("\r\n第%d次超时时间设置尝试失败，准备继续尝试设置\r\n",i+1);
		}
	 } 
//	 esp8266_get_ip(wifi_ip_address);
	 delay_ms(500);
	 esp8266_Get_IP();		//获取被分配的IP地址
	 u2_printf("\r\nready\r\n");
}




/***********************************************************************
 函数名    ：ESP8266_apsta_Init
 功能      ：初始化esp8266的AP服务器-TCP服务器模式
 参数      ：n,失败重连次数
 返回值    ：无
 其他      ：
***********************************************************************/
void ESP8266_apsta_Init(u8 n)
{
   u8 i;
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+CWMODE=3","OK",100))		//发送指令：AT+CWMODE=3
		 {
		   u2_printf("\r\n模式设置成功\r\n");
			 break;
		 }
		 else
		 {
		   u2_printf("\r\n第%d次模式设置尝试失败，准备继续尝试设置\r\n",(i+1));
		 }
	 }
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_send_cmd("AT+RST","OK",100))			//发送指令：AT+RST
		 {
		   u2_printf("\r\n重启设置成功\r\n");
			 break;
		 }
		 else
		 {
		   u2_printf("\r\n第%d次重启尝试失败，准备继续重启设置\r\n",i+1);
		 }	 
	 }
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_SET_AP(wifi_ssid,wifi_ssid_password,WPA_WPA2_PSK,500))				//设置网络名称，密码，加密方式
		 {
		   u2_printf("\r\nAP设置成功\r\n");
			 break;
		 }
		 else
		 {
		   u2_printf("\r\n第%d次AP设置尝试失败，准备继续尝试设置\r\n",i+1);
		 }
		 delay_ms(100);
	 }
	 
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(ESP8266_SET_IP(wifi_ap_ip,60))		//设置IP
			{
				u2_printf("\r\nIP设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次IP设置尝试失败，准备继续尝试设置\r\n",i+1);
			}		
			delay_ms(1000);			
	 }
	 
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
		 if(atk_8266_SET_STA(wifi_station,wifi_station_password,3000))		//给定路由器名称和密码
		 {
		   u2_printf("\r\nWIFI-STATION设置成功\r\n");
			 delay_ms(9000);
			 break;
		 }
		 else
		 {
		   u2_printf("\r\n第%d次WIFI-STATION设置尝试失败，准备继续尝试设置\r\n",i+1);
		 }
		 memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);	          //清除串口4接收缓存区	
		 delay_ms(8000);
	 }

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
			if(atk_8266_send_cmd("AT+CIPMUX=1","OK",450))
			{
				 u2_printf("\r\n开启多路连接成功\r\n");
				 break;
			}
				else
			{
				u2_printf("\r\n第%d次多路连接失败，准备继续尝试设置\r\n",i+1);		
			}
		 memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);			          //清除串口4接收缓存区	
		 delay_ms(1000);
	 }	

	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {		 
			if(ESP8266_SET_PORT(wifi_ap_port,50))			//设置端口号
			{
				u2_printf("\r\n端口设置成功\r\n");
				break;
			}
			else
			{
				u2_printf("\r\n第%d次端口设置尝试失败，准备继续尝试设置\r\n",i+1);
			}
			delay_ms(1000);					
	 }	 

	 
	 for(i=0;i<n;i++)		//最大循环次数设置为参数n
	 {
	   delay_ms(20);
		 if(atk_8266_send_cmd("AT+CIPSTO=2200","OK",500))
		 {
		   u2_printf("\r\n超时时间设置成功\r\n");
			 break;
		 }
		 else
		 {
		   u2_printf("\r\n第%d次超时时间设置尝试失败，准备继续尝试设置\r\n",i+1);
		 }
		 memset(USART3_RX_BUF,0,USART3_MAX_RECV_LEN);			          //清除串口4接收缓存区	
		 delay_ms(1000);
	 } 
	 esp8266_Get_IP();		//获取IP
	 u2_printf("\r\nready\r\n");
}



