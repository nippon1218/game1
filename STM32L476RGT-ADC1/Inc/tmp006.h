#ifndef __tmp006_H
#define __tmp006_H
#include "sys.h"
#include "myiic.h"

#define tmp006_ADDR 	0X82


u8 tmp006_Init(void);           
u16 tmp006_ReadTwoByte(u8 daddr);

#endif