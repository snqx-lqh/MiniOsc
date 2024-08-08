# 快速入门

## ADC转换器简介

adc转换器的功能就是将模拟电压转换为数字信号，比如一个12位的ADC转换器，然后ADC最高检测的电压是3.3V，那么他就会把单片机引脚检测到的0~3.3V转换成单片机里面能处理的数字0~4095，其实就是2的12次方。

## ADC配置

这里主要讲配置和使用，下面是实际的配置代码。

```c
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
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由定时器2的通道2触发（只有在上升沿时可以触发）
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
		ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
		ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

		ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1		
		
		ADC_ResetCalibration(ADC1);	//使能复位校准  
		 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );//ADC1通道1,采样时间为28.5周期	 
		 
		ADC_ResetCalibration(ADC1);//复位较准寄存器
		 
		while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
		
		ADC_StartCalibration(ADC1);	 //开启AD校准
	 
		while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
	}
}
```

引脚配置部分，将PA1配置成了模拟输入引脚。

ADC配置部分，显示初始化了ADC的配置部分，然后进行了一次ADC的复位校准。

这里有一句函数比较重要

```c
ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );//ADC1通道1,采样时间为28.5周期	
```

它说明了我们现在配置的采样通道，是ADC1的通道1，采样1次，然后转换的时间是28.5个周期

根据手册，ADC的转换时间Tcovn=采样时间+12.5 个周期，那我们这里的设置就是41个周期。前面我们设置了ADC的分频RCC_ADCCLKConfig(RCC_PCLK2_Div6);，这个函数将ADC的工作频率设置成了72/6=12Mhz，那么我们转换一次ADC所用的时间就是41x1/12M s= 3.4167us。

ADC配置完成后，我们就要写启动单次的转化功能

```c
//获得ADC值
u16 get_adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
```

这个就是一次的ADC转换，它先设置转换的规则，比如通道周期，然后开始转换，等待转换完成后，将转换后的数据返回回来。

我们在主函数中使用一次这个代码

```C
int main(void)
{	
	uint16_t adc_value = 0;
	float    voltage   = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	bsp_exti_init();
	bsp_timer_init(5000,72);
	bsp_usart1_init(115200);
	delay_init();
	bsp_adc_init();
	
	osc_ui_init();
	
	while(1)
	{
		adc_value = get_adc(1);
		printf("adc_value : %d ",adc_value);
		voltage = adc_value * 3.3/4095;
		printf("voltage_value : %f\r\n",voltage);
		delay_ms(500);
	}
}
```

然后就可以使用串口查看这个转换的数据值了。

## ADC的DMA传输

但是我们的ADC转换比较耗时间，每次转换就处理要占用时间相对来说比较长，所以我们引入了DMA。DMA就是单片机的一个外设，他经过配置后可以实现从一个内存地址往另外一个内存地址不断的搬运数据，而且不占用CPU的使用，这段搬运时间CPU就可以去做其他的事情。

DMA的使用的话，初始化代码如下

```c
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
```

DMA的初始化，DMA对应通道和ADC是有对应的，这个要查询数据手册。

DMA的初始化要确定的点，有以下几个

1、哪个DMA通道进行传输，这个可以查手册

2、DMA的传输方向，有三种，外设到内存、内存到外设、内存到内存，我们这里是外设到内存

3、外设和内存地址递增与否，这里外设不增，内存增，因为外设地址是不变的，然后移动数据从外设到内存的话，第一片内存放了数据，第二次再来数据就要放到第二片了。

4、要转移的数据量，就是一次DMA传输的数据量，传输完成后就会进入DMA中断。

DMA初始化完成后，还要修改ADC的初始化配置

```c
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
		 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );//ADC1通道1,采样时间为28.5周期	 
		 
		ADC_ResetCalibration(ADC1);//复位较准寄存器
		 
		while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
		
		ADC_StartCalibration(ADC1);	 //开启AD校准
	 
		while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
	 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
		
		ADC_DMACmd(ADC1, ENABLE);	//ADC的DMA功能使能
	}
}
```

相比之前，修改了两个部分，一个是ADC的转换触发，ADC_ExternalTrigConv这里使用的是ADC_ExternalTrigConv_T2_CC2定时器通道触发，意思就是让定时器2来产生一个脉冲，然后有一个脉冲就会触发一次转换。还有一个新增就是打开ADC的DMA功能ADC_DMACmd(ADC1, ENABLE);在这里我们初始化的adc转换周期是ADC_SampleTime_28Cycles5，那么我们每次定时器触发后他的转换时间都要有前面说的3.几us

修改这里后，我们发现使用了定时器的触发功能，那么我们需要初始化一个定时器的触发功能。

```c
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
```

这个初始化里面，定时器的基本配置是一样的

定时器2的通道2配置是，让定时器2的通道2输出PWM脉冲，以此来触发ADC的转换。比如加入给的参数是bsp_timer2_init(720,1,2);，那么定时器的周期就是720x1/72000000 s，就是10us，那么就会10us触发一次ADC转换，采样频率就是100000KHz,

将这个几个部分都处理完了，我们就可以处理DMA中断的相关函数了。

```c
/*
DMA1中断AD采集中断，每采集到1024个值便进入中断
*/
void DMA1_Channel1_IRQHandler(void) 
{
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{                   
		adc_flag=1;                        //将AD采集完成标志位归1
		DMA_Cmd(DMA1_Channel1, DISABLE);   //失能AD采集通道     
	}
	DMA_ClearITPendingBit(DMA1_IT_TC1);         
}
```

每次接受完一次数据，我们将DMA的接受关闭，然后再在While中处理好相关的数据后，再使能DMA的传输。用adc_flag这个变量来确定数据传输完了。

```c
uint8_t adc_flag = 0;

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	bsp_exti_init();
	bsp_timer_init(5000,72);
	bsp_usart1_init(115200);
	delay_init();
	bsp_adc_init();
	
	osc_ui_init();
	
	while(1)
	{
		while(adc_flag==0);     //当采集完成后跳出循环
		
		//做数据处理		
		
		adc_flag=0;                        //将标志位置0
		DMA_Cmd(DMA1_Channel1, ENABLE);    //使能AD采集
	}
}
```

我们可以尝试将ADC采集的数据，替换到之前显示的数据。记得在bsp_adc.h中

```c
extern volatile uint16_t ADC_ConvertedValue[];
```

这样才能在别的文件中调用这个变量。

将osc_ui.c的show_wave函数修改。并且去掉他在osc_ui_init中的使用，放到osc_ui_loop中。

```c
/*
显示波形
*/
void show_wave()
{
	int16_t i;
	for(i=0;i<239;i++)
	{				lcd_drawline(i,ADC_ConvertedValue[i]*200/4096,i+1,ADC_ConvertedValue[i+1]*200/4096)	
	}
}
```

记得在主函数中初始化刚刚定义的

```c
bsp_adc_init();
bsp_timer2_init(720,1,2);
adc_dma_init();
```

一切完成下载后，会发现上面的波形显示后就一直在那了，我们需要一个处理来将上次显示的波型删去，这里的处理是，将上次画的波形数据保留下来，下次再画的时候先清除。处理以下

```c
/*
显示波形
*/
void show_wave()
{
	int16_t i;
	for(i=0;i<239;i++)
	{
		lcd_drawline(i,ADC_ConvertedValue[i]*200/4096,i+1,ADC_ConvertedValue[i+1]*200/4096);
		show_buff[i] = ADC_ConvertedValue[i]*200/4096;		
	}
	show_buff[239] =  ADC_ConvertedValue[239]*200/4096;
}

/*
清除上一次波形
原理为先使用白线将上一次所画的线再画一遍，达到清除作用
然后再将画笔转换画波形的颜色
*/
void show_wave_clr()
{
	int16_t i;
	POINT_COLOR = BLACK;
	for(i=0;i<239;i++)
	{
		lcd_drawline(i,show_buff[i],i+1,show_buff[i+1]);
	}
	POINT_COLOR = YELLOW;
}
```

然后就在每次画线前先清除，下载下去后会发现他有时候会把原来画的方框擦除，我们需要在画线前，再画一次框。修改后如下

```c
void osc_ui_loop()
{
	show_wave_clr();
	show_ui();
	show_wave();
}
```

