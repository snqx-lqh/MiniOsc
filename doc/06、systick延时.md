# 快速入门

## systick基础

这是一个内部的计数器，使用的话可以先直接使用，后面学习细节

## 文件配置

在使用的时候，只需要初始化后，就可以直接使用延时函数。建立bsp_delay.c和bsp_delay.h文件

```c
#include "bsp_delay.h"

static u8  fac_us=0;							//us延时倍乘数
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

void delay_init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8
    fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数
}

void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD=nus*fac_us; 					//时间加载
    SysTick->VAL=0x00;        					//清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
    do
    {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL =0X00;      					 //清空计数器
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;							//清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
    do
    {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL =0X00;       					//清空计数器
}
```

在main函数中用led闪烁测试

