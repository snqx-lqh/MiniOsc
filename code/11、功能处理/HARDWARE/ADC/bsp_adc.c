#include "bsp_adc.h"

void bsp_adc_init()
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE );	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE );	  //使能ADC1通道时钟
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//PA1 作为模拟通道输入引脚                         
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
	}
	{
		ADC_InitTypeDef ADC_InitStructure; 
		
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
		ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1工作在独立模式
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在非连续转换模式
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//转换由定时器2的通道2触发（只有在上升沿时可以触发）
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
		ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
		ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

		ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1		
		
		ADC_ResetCalibration(ADC1);	//使能复位校准  
		 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );//ADC1通道1,采样时间为239.5周期	 
		 
		ADC_ResetCalibration(ADC1);//复位较准寄存器
		 
		while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
		
		ADC_StartCalibration(ADC1);	 //开启AD校准
	 
		while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
		
		ADC_DMACmd(ADC1, ENABLE);	//ADC的DMA功能使能
	}
}

//获得ADC值
u16 get_adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

///******************************************************************
//函数名称:bsp_timer2_init(u16 arr,u16 psc,u16 pulse)
//函数功能:定时器3，PWM输出模式初始化函数
//参数说明: arr：重装载值
//		   psc：预分频值
//         pulse:占空比值
//备    注:通过TIM2-CH2的PWM输出触发ADC采样
//*******************************************************************/ 	
void bsp_timer2_init(u16 arr,u16 psc,u16 pulse)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		//初始化TIM2
		TIM_TimeBaseStructure.TIM_Period = arr-1;   //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
		TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值 
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	}
	{
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		//初始化TIM2 Channel2 PWM模式	 
		TIM_OCInitStructure.TIM_OCMode =      TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
		TIM_OCInitStructure.TIM_OCPolarity =  TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
		TIM_OCInitStructure.TIM_Pulse=        pulse;
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2

		TIM_CtrlPWMOutputs(TIM2, ENABLE);//使能PWM输出
	}
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
}

volatile uint16_t ADC_ConvertedValue[1024];			    //ADC采样的数据

void adc_dma_init() // cpar为外设地址，cmar为内存地址，cndtr为搬运的数据个数（对应于数组长度）
{
    DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
    DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & ADC1->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存//
	DMA_InitStructure.DMA_BufferSize = 1024;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //ADC1匹配DMA通道1
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	//使能DMA传输中断	
	
	//配置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	DMA_Cmd(DMA1_Channel1,ENABLE);//使能DMA通道
}
