#ifndef __bma222_H
#define __bma222_H
#include "sys.h"
#include "myiic.h"

#define bma222_ADDR 	0X18

extern u8 bmadata[3];


void bma222_Init(void);
u8 bma222_ReadOneByte(void);
void bma222_WriteOneByte(u8 DataToWrite);   
void bma222_WriteBit(u8 bit,u8 sta);     
u8 bma222_ReadBit(u8 bit); 



void bma222_ReadAcc(void);



#endif