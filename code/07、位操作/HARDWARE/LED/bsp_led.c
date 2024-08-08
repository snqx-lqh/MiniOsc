#include "bsp_led.h"

void bsp_led_init(void)
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	//初始化IO口
	{
		GPIO_InitTypeDef GPIO_InitStruct;    //定义一个IO口参数结构体
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;        //选择想要配置的端口
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   //选择想要的输出形式
		GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;   //选择想要输出的速度
		
		GPIO_Init(GPIOA,&GPIO_InitStruct);   //根据参数结构体，初始化该参数
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;        //选择想要配置的端口
		GPIO_Init(GPIOD,&GPIO_InitStruct);   //根据参数结构体，初始化该参数
	}
}

void led0_on()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void led0_off()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void led1_on()
{
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);
}

void led1_off()
{
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
}