#include "delay2.h"
#include "sys.h"


static uint8_t fac_us2 = 0;
static uint16_t fac_ms2 = 0;

#define SYSCLK2 8  //主频：8MHz

void delay2_init(void) {
    SysTick->CTRL &= 0xfffffffb;//控制寄存器，选择外部时钟即系统时钟的八分之一（HCLK/8；72M/8=9M）
    fac_us2 = SYSCLK2 / 8;    //为系统时钟的1/8
    fac_ms2 = (uint16_t) fac_us2 * 1000;//代表每个ms需要的systick时钟数
}

void delay2_us(uint32_t nus)//延时us
{
    uint32_t temp;
    SysTick->LOAD = nus * fac_us2;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    do {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));
    SysTick->CTRL = 0x00;       //关闭计数器
    SysTick->VAL = 0X00;
}

void delay2_ms(uint32_t nms)//延时ms
{
    for (;nms>0;nms--){
        delay2_us(1000);
    }
}

