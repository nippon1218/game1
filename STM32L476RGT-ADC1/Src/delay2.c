#include "delay2.h"
#include "sys.h"


static uint8_t fac_us2 = 0;
static uint16_t fac_ms2 = 0;

#define SYSCLK2 8  //��Ƶ��8MHz

void delay2_init(void) {
    SysTick->CTRL &= 0xfffffffb;//���ƼĴ�����ѡ���ⲿʱ�Ӽ�ϵͳʱ�ӵİ˷�֮һ��HCLK/8��72M/8=9M��
    fac_us2 = SYSCLK2 / 8;    //Ϊϵͳʱ�ӵ�1/8
    fac_ms2 = (uint16_t) fac_us2 * 1000;//����ÿ��ms��Ҫ��systickʱ����
}

void delay2_us(uint32_t nus)//��ʱus
{
    uint32_t temp;
    SysTick->LOAD = nus * fac_us2;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    do {
        temp = SysTick->CTRL;
    } while (temp & 0x01 && !(temp & (1 << 16)));
    SysTick->CTRL = 0x00;       //�رռ�����
    SysTick->VAL = 0X00;
}

void delay2_ms(uint32_t nms)//��ʱms
{
    for (;nms>0;nms--){
        delay2_us(1000);
    }
}

