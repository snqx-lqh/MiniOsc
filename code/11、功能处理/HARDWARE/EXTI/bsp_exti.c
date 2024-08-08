#include "bsp_exti.h"

#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_dac.h"
#include "osc_ui.h"

int16_t osc_mode = 0;   //按键控制模式 0改变波形 1改变DA产生频率 2改变采样频率 3改变DA产生幅值
int16_t wave_mode = 0;  //波形模式     0正弦波   1方波
int16_t stop_mode = 0;
int16_t divy_mode = 0;

void key_up_down()
{
    osc_mode++;        //每次进入中断模式++
    if(osc_mode > 5)
    {
        osc_mode = 0;
    }
}

void key_0_down()
{
    if(osc_mode == 0)
    {
        wave_mode = 0;		                        //改变为正弦波
		da_change = 1;                              //da改变标志位置1
    } else if(osc_mode == 1)
    {
        f_set-=100;                                 //将DA产生波形的频率减100
		TIM4->ARR = 720000/f_set-1;                 //重新设置定时器4的计数值，达到改变输出频率的变化
    } else if(osc_mode == 2)
	{
		OSR-=10000;                                   //将AD采集的频率减10000
		DIVX = 1.0f*1000000*20/OSR;
		TIM2->ARR = 72000000/OSR-1;                   //重新设置定时器2的计数值，达到改变采样频率的变化
	}else if(osc_mode == 3)
	{
		DA_VALUE_RATE = DA_VALUE_RATE - 0.1;        //将幅值比例系数减0.1
		if(DA_VALUE_RATE < 0)                       //假如比例系数小于0，则比例系数为0
			DA_VALUE_RATE = 0;
		da_change = 1;                              //da改变标志位置1
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
        wave_mode = 1;                               //改变为三角波
		da_change = 1;                               //da改变标志位置1
    } else if(osc_mode == 1)
    {
        f_set+=100;                                    //将DA产生波形的频率加100
		TIM4->ARR = 720000/f_set-1;                    //重新设置定时器4的计数值，达到改变输出频率的变化
    } else if(osc_mode == 2)
	{
		OSR+=10000;                                    //将AD采集的频率加10000
		DIVX = 1.0f*1000000*20/OSR;
		TIM2->ARR = 72000000/OSR-1;                      //重新设置定时器2的计数值，达到改变采样频率的变化 
	}else if(osc_mode == 3)
	{
		DA_VALUE_RATE = DA_VALUE_RATE + 0.1;         //将幅值比例系数加0.1
		if(DA_VALUE_RATE > 1)                        //假如比例系数大于1，则比例系数为1
			DA_VALUE_RATE = 1;
		da_change = 1;                               //da改变标志位置1  
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
