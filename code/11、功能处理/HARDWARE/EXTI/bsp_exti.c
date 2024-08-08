#include "bsp_exti.h"

#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_dac.h"
#include "osc_ui.h"

int16_t osc_mode = 0;   //��������ģʽ 0�ı䲨�� 1�ı�DA����Ƶ�� 2�ı����Ƶ�� 3�ı�DA������ֵ
int16_t wave_mode = 0;  //����ģʽ     0���Ҳ�   1����
int16_t stop_mode = 0;
int16_t divy_mode = 0;

void key_up_down()
{
    osc_mode++;        //ÿ�ν����ж�ģʽ++
    if(osc_mode > 5)
    {
        osc_mode = 0;
    }
}

void key_0_down()
{
    if(osc_mode == 0)
    {
        wave_mode = 0;		                        //�ı�Ϊ���Ҳ�
		da_change = 1;                              //da�ı��־λ��1
    } else if(osc_mode == 1)
    {
        f_set-=100;                                 //��DA�������ε�Ƶ�ʼ�100
		TIM4->ARR = 720000/f_set-1;                 //�������ö�ʱ��4�ļ���ֵ���ﵽ�ı����Ƶ�ʵı仯
    } else if(osc_mode == 2)
	{
		OSR-=10000;                                   //��AD�ɼ���Ƶ�ʼ�10000
		DIVX = 1.0f*1000000*20/OSR;
		TIM2->ARR = 72000000/OSR-1;                   //�������ö�ʱ��2�ļ���ֵ���ﵽ�ı����Ƶ�ʵı仯
	}else if(osc_mode == 3)
	{
		DA_VALUE_RATE = DA_VALUE_RATE - 0.1;        //����ֵ����ϵ����0.1
		if(DA_VALUE_RATE < 0)                       //�������ϵ��С��0�������ϵ��Ϊ0
			DA_VALUE_RATE = 0;
		da_change = 1;                              //da�ı��־λ��1
	}else if(osc_mode == 4)
	{
		divy_mode--;
		if(divy_mode<0)
			divy_mode = 0;
	}else if(osc_mode == 5)
	{
		stop_mode = 0;
	}
}

void key_1_down()
{
    if(osc_mode == 0)
    {
        wave_mode = 1;                               //�ı�Ϊ���ǲ�
		da_change = 1;                               //da�ı��־λ��1
    } else if(osc_mode == 1)
    {
        f_set+=100;                                    //��DA�������ε�Ƶ�ʼ�100
		TIM4->ARR = 720000/f_set-1;                    //�������ö�ʱ��4�ļ���ֵ���ﵽ�ı����Ƶ�ʵı仯
    } else if(osc_mode == 2)
	{
		OSR+=10000;                                    //��AD�ɼ���Ƶ�ʼ�10000
		DIVX = 1.0f*1000000*20/OSR;
		TIM2->ARR = 72000000/OSR-1;                      //�������ö�ʱ��2�ļ���ֵ���ﵽ�ı����Ƶ�ʵı仯 
	}else if(osc_mode == 3)
	{
		DA_VALUE_RATE = DA_VALUE_RATE + 0.1;         //����ֵ����ϵ����0.1
		if(DA_VALUE_RATE > 1)                        //�������ϵ������1�������ϵ��Ϊ1
			DA_VALUE_RATE = 1;
		da_change = 1;                               //da�ı��־λ��1  
	}else if(osc_mode == 4)
	{
		divy_mode++;
		if(divy_mode>2)
			divy_mode = 2;
	}else if(osc_mode == 5)
	{
		stop_mode = 1;
	}
}


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
        delay_ms(10);
        if(get_key_wkup_value() == 1)
            key_up_down();
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) == SET)
    {
        delay_ms(10);
        if(get_key_0_value() == 0)
            key_0_down();
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line15) == SET)
    {
        delay_ms(10);
        if(get_key_1_value() == 0)
            key_1_down();
    }
    EXTI_ClearITPendingBit(EXTI_Line15);
}
