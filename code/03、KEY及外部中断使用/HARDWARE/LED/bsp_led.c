#include "bsp_led.h"

void bsp_led_init(void)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	//��ʼ��IO��
	{
		GPIO_InitTypeDef GPIO_InitStruct;    //����һ��IO�ڲ����ṹ��
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8;        //ѡ����Ҫ���õĶ˿�
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;   //ѡ����Ҫ�������ʽ
		GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;   //ѡ����Ҫ������ٶ�
		
		GPIO_Init(GPIOA,&GPIO_InitStruct);   //���ݲ����ṹ�壬��ʼ���ò���
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_2;        //ѡ����Ҫ���õĶ˿�
		GPIO_Init(GPIOD,&GPIO_InitStruct);   //���ݲ����ṹ�壬��ʼ���ò���
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