#include "bsp_dac.h"

#include "bsp_exti.h"


//���Ҳ�����
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

//���ǲ�����
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


int16_t DAC_Value_BUFF1[100];      //���������Ҳ������           
int16_t DAC_Value_BUFF2[100];      //���������������

int16_t da_change;

float DA_VALUE_RATE = 1;

/*
DA�����������ı�
�����°����򴥷�һ�Σ��޸ķ�ֵ���߲���
*/
void da_change_wave()
{
	int16_t i;
	if(da_change == 1)                   //�����־λΪ1
	{
		DMA_Cmd(DMA2_Channel3, DISABLE); // ʧ��DA������DMA
		if(wave_mode == 0)               // ���粨��Ϊ���Ҳ�
		{
			for(i=0;i<100;i++)           //�ֱ𰴱����ı�DA�������ֵ
			{
				DAC_Value_BUFF1[i] = DA_VALUE_RATE * DAC_Value[i];
			}
			DMA2_Channel3->CMAR = (u32)DAC_Value_BUFF1;	//��DA���DMA�Ļ���ַ�������Ҳ����BUFF
		}else if(wave_mode == 1)         // ���粨��Ϊ���ǲ�
		{
			for(i=0;i<100;i++)           //�ֱ𰴱����ı�DA�������ֵ
			{
				DAC_Value_BUFF2[i] = DA_VALUE_RATE * DAC_Value1[i];
			}
			DMA2_Channel3->CMAR = (u32)DAC_Value_BUFF2;	//��DA���DMA�Ļ���ַ���ɷ������BUFF
		}
		DMA_Cmd(DMA2_Channel3, ENABLE);  // ʹ��DA������DMA
		da_change = 0;                   //����־λ��0
	}
}


void dac_all_init(u16 arr,u16 psc)
{
	int16_t i;
	for(i=0;i<100;i++)  //��ʼ�����Ҳ����BUFF�����ֵ
	{
		DAC_Value_BUFF1[i] = DA_VALUE_RATE * DAC_Value[i];
	}
	for(i=0;i<100;i++)  //��ʼ�����ǲ����BUFF�����ֵ
	{
		DAC_Value_BUFF2[i] = DA_VALUE_RATE * DAC_Value1[i];
	}
	bsp_dac_init();    
	bsp_timer4_init(arr,psc);
	dac_dma_init(); 
}

//DACͨ��1�����ʼ��
void bsp_dac_init(void)
{ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //ʹ��DACͨ��ʱ�� 
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
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;          //��ʹ�ò��η���
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	      //DAC1�������ر� BOFF1=1
		DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1

		DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1
		DAC_DMACmd(DAC_Channel_1, ENABLE);
	}
}

/******************************************************************
��������:bsp_timer4_init(u16 arr,u16 psc)
��������:��ʱ��4��ʼ������
����˵��:arr����װ��ֵ
		 psc��Ԥ��Ƶֵ
��    ע:���ڴ���DACת�����������һ����ǲ�
*******************************************************************/ 	  
void bsp_timer4_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);//�������跽ʽΪ���´���
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
							 
}

void dac_dma_init() 
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); // ��Ҫ���ܶ�ʱ��ֻ���Ÿ�DMAͨ��������ʹ�ܶ�Ӧʱ�ӣ�����DMA������

    DMA_DeInit(DMA2_Channel3); // ��ʼ��

    DMA_StructInit(&DMA_InitStructure); // ��ʼ��
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // ���򣺴��ڴ浽����
    DMA_InitStructure.DMA_BufferSize = 100; // ָ��ÿ��DMA��Ҫ���˵����ݸ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // �����ַ���䣨�̶�ΪDAC�Ĵ�����ַ��
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // �ڴ��ַ�������������飩
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // �������ݳ���Ϊ16bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // �ڴ����ݳ���Ϊ16bit
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // ����Ϊ�����ȼ���ʹ�õ���DMAʱ��Ӱ�죩
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // ���ڴ浽�ڴ�ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // ѭ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & (DAC->DHR12R1); // ������ʼ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&DAC_Value_BUFF1; // �ڴ���ʼ��ַ

    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE); // ʹ��
}