#include "bsp_dac.h"

#include "bsp_exti.h"


//正弦波数据
int16_t DAC_Value[100] ={
	2048,2176,2304,2431,2557,2680,2801,2919,3034,3144,
	3251,3352,3449,3540,3625,3704,3776,3842,3900,3951,
	3995,4031,4059,4079,4091,4095,4092,4080,4060,4032,
	3996,3952,3901,3843,3778,3706,3627,3542,3451,3355,
	3253,3147,3037,2922,2804,2683,2560,2434,2307,2179,
	2051,1922,1794,1667,1542,1418,1297,1179,1064,953,
	847,745,648,557,472,393,321,255,196,145,
	101,65,37,16,4,0,3,15,35,63,98,
	141,192,250,315,387,466,551,641,738,839,
	945,1056,1170,1288,1409,1532,1658,1784,1912	
};

//三角波数据
int16_t DAC_Value1[100] ={
0,81,163,245,327,409,491,573,655,
737,819,901,983,1064,1146,1228,
1310,1392,1474,1556,1638,1720,
1802,1884,1966,2047,2129,2211,
2293,2375,2457,2539,2621,2703,
2785,2867,2949,3031,3112,3194,
3276,3358,3440,3522,3604,3686,
3768,3850,3932,4014,4095,4014,
3932,3850,3768,3686,3604,3522,
3440,3358,3276,3194,3112,3031,
2949,2867,2785,2703,2621,2539,
2457,2375,2293,2211,2129,2048,
1966,1884,1802,1720,1638,1556,
1474,1392,1310,1228,1146,1064,
983,901,819,737,655,573,491,
409,327,245,163,81
};


int16_t DAC_Value_BUFF1[100];      //拿来做正弦波的输出           
int16_t DAC_Value_BUFF2[100];      //拿来做方波的输出

int16_t da_change;

float DA_VALUE_RATE = 1;

/*
DA输出缓冲数组改变
若按下按键则触发一次，修改幅值或者波形
*/
void da_change_wave()
{
	int16_t i;
	if(da_change == 1)                   //假如标志位为1
	{
		DMA_Cmd(DMA2_Channel3, DISABLE); // 失能DA产生的DMA
		if(wave_mode == 0)               // 假如波形为正弦波
		{
			for(i=0;i<100;i++)           //分别按比例改变DA输出缓冲值
			{
				DAC_Value_BUFF1[i] = DA_VALUE_RATE * DAC_Value[i];
			}
			DMA2_Channel3->CMAR = (u32)DAC_Value_BUFF1;	//将DA输出DMA的基地址换成正弦波输出BUFF
		}else if(wave_mode == 1)         // 假如波形为三角波
		{
			for(i=0;i<100;i++)           //分别按比例改变DA输出缓冲值
			{
				DAC_Value_BUFF2[i] = DA_VALUE_RATE * DAC_Value1[i];
			}
			DMA2_Channel3->CMAR = (u32)DAC_Value_BUFF2;	//将DA输出DMA的基地址换成方波输出BUFF
		}
		DMA_Cmd(DMA2_Channel3, ENABLE);  // 使能DA产生的DMA
		da_change = 0;                   //将标志位置0
	}
}


void dac_all_init(u16 arr,u16 psc)
{
	int16_t i;
	for(i=0;i<100;i++)  //初始化正弦波输出BUFF里面的值
	{
		DAC_Value_BUFF1[i] = DA_VALUE_RATE * DAC_Value[i];
	}
	for(i=0;i<100;i++)  //初始化三角波输出BUFF里面的值
	{
		DAC_Value_BUFF2[i] = DA_VALUE_RATE * DAC_Value1[i];
	}
	bsp_dac_init();    
	bsp_timer4_init(arr,psc);
	dac_dma_init(); 
}

//DAC通道1输出初始化
void bsp_dac_init(void)
{ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //使能PORTA通道时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //使能DAC通道时钟 
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		  
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
	}
	{
		DAC_InitTypeDef DAC_InitType;
		DAC_InitType.DAC_Trigger=DAC_Trigger_T4_TRGO;	                  //
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;          //不使用波形发生
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//屏蔽、幅值设置
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	      //DAC1输出缓存关闭 BOFF1=1
		DAC_Init(DAC_Channel_1,&DAC_InitType);	 //初始化DAC通道1

		DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC1
		DAC_DMACmd(DAC_Channel_1, ENABLE);
	}
}

/******************************************************************
函数名称:bsp_timer4_init(u16 arr,u16 psc)
函数功能:定时器4初始化函数
参数说明:arr：重装载值
		 psc：预分频值
备    注:用于触发DAC转换，产生正弦或三角波
*******************************************************************/ 	  
void bsp_timer4_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);//触发外设方式为更新触发
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
							 
}

void dac_dma_init() 
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); // 重要，很多时候只顾着改DMA通道而忘记使能对应时钟，导致DMA不工作

    DMA_DeInit(DMA2_Channel3); // 初始化

    DMA_StructInit(&DMA_InitStructure); // 初始化
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // 方向：从内存到外设
    DMA_InitStructure.DMA_BufferSize = 100; // 指定每轮DMA需要搬运的数据个数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不变（固定为DAC寄存器地址）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址自增（遍历数组）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据长度为16bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 内存数据长度为16bit
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置为高优先级（使用单个DMA时无影响）
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 非内存到内存模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 循环
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & (DAC->DHR12R1); // 外设起始地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DAC_Value_BUFF1; // 内存起始地址

    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE); // 使能
}