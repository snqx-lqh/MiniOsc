#include "bsp_key.h"

void bsp_key_init()
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//初始化IO口
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
		
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
		
		GPIO_Init(GPIOC,&GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_15;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	}
}

uint8_t get_key_wkup_value(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}

uint8_t get_key_0_value(void)
{
	return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5);
}

uint8_t get_key_1_value(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);
}
