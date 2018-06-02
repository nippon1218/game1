#ifndef __ESP8266_H
#define __ESP8266_H	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	 	 	 	 	    
#include "string.h"   

#define SSID "Medical_Nursing_Bed"    //AP模式下的热点名称
#define PASS "12345678"               //热点的密码

//WiFi热点名称
#define WIFI_STATION "mitsubishi"     //要加入的WiFi站点名称
#define PASSWORD "884869japan"        //相应的密码

#define ECN WPA_WPA2_PSK               
#define port 8086                     //端口号
#define WPA_WPA2_PSK 4                //加密模式


void atk_8266_at_response(u8 mode);                           //用串口2打印WiFi接收到的数据。
u8* atk_8266_check_cmd(u8 *str);                              //判断UART4_RX_BUF数组中有无指定字符串 
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);           //发送WiFi指令
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);         //发送数据
u8 atk_8266_quit_trans(void);                                 //退出透传
u8 atk_8266_SET_AP(u8 *ssid,u8 *password,u8 mode,u16 timeout);//esp8266设置AP模式
u8 atk_8266_SET_IPPORT(u16 timeout);                          //esp8266设置多路连接
u8 atk_8266_SET_STA(u8 *ssid,u8 *password,u16 timeout);       //esp8266设置STA模式
u8 atk_8266_SET_PORT(u16 timeout);                            //设置端口号

u8 ESP8266_SET_IP(u8 *Ip,u16 timeout);                        //设置IP地址
u8 ESP8266_SET_PORT(u16 Port,u16 timeout);                    //设置端口号

void atk_8266_get_wanip(u8* ipbuf);             //获取ipbuf中，介于"\"和"\"中的数据
void esp8266_get_ip(u8* ipbuf);                 //从ipbuf中获取路由器分配给STM的IP地址
void esp8266_AP_Init(void);                     //初始化esp8266的AP模式
void esp8266_STA_SERVER_Init(void);             //初始化esp8266的STA模式
void esp8266_Get_IP(void);                      //获取IP地址
void esp8266_send_data(u8 *data);               //发送数据给设备0

void ESP8266_apsta_Init(u8 n);                  //初始化esp8266的AP服务器-TCP服务器模式
void ESP8266_AP_Init(u8 n);		                //初始化esp8266的AP模式


#endif

