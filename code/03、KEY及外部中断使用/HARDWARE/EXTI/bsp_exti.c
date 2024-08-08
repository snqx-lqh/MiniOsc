#include "bsp_exti.h"

#include "bsp_key.h"
#include "bsp_led.h"

void bsp_exti_init()
{
	//初始化按键
	bsp_key_init();
	//打开复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//中断线初始化
	{
		EXTI_InitTypeDef EXTI_InitStructure;
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line0;	           //中断线
		EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	   //中断模式
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;     //触发方式
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		
		EXTI_Init(&EXTI_InitStructure);
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line5;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
		EXTI_Init(&EXTI_InitStructure);
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line15;
		EXTI_Init(&EXTI_InitStructure);
		
		//初始化中断线
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	}
	//NVIC初始化
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能外部中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        0;	//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		
		NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能外部中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        1;	//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		
		NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能外部中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        2;	//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
		
		NVIC_Init(&NVIC_InitStructure); 
	}
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		led0_off();
	}
	EXTI_ClearITPendingBit(EXTI_Line0); 
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) == SET)
	{
		led0_on();
	}
	EXTI_ClearITPendingBit(EXTI_Line5); 
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		
	}
	EXTI_ClearITPendingBit(EXTI_Line15); 
}
