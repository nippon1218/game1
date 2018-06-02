#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"



extern TIM_HandleTypeDef TIM7_Handler;  //定时器7句柄

void TIM7_Init(u16 arr,u16 psc);        //定时器7初始化函数，WiFi专用




#endif









