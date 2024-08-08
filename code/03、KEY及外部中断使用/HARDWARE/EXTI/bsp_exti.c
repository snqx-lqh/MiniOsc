#include "bsp_exti.h"

#include "bsp_key.h"
#include "bsp_led.h"

void bsp_exti_init()
{
	//��ʼ������
	bsp_key_init();
	//�򿪸���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//�ж��߳�ʼ��
	{
		EXTI_InitTypeDef EXTI_InitStructure;
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line0;	           //�ж���
		EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	   //�ж�ģʽ
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;     //������ʽ
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		
		EXTI_Init(&EXTI_InitStructure);
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line5;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		
		EXTI_Init(&EXTI_InitStructure);
		
		EXTI_InitStructure.EXTI_Line    = EXTI_Line15;
		EXTI_Init(&EXTI_InitStructure);
		
		//��ʼ���ж���
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	}
	//NVIC��ʼ��
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ���ⲿ�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        0;	//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		
		NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ���ⲿ�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        1;	//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		
		NVIC_Init(&NVIC_InitStructure); 
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ���ⲿ�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ� 
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =        2;	//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
		
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
