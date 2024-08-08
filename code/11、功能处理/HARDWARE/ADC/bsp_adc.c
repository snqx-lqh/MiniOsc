#include "bsp_adc.h"

void bsp_adc_init()
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE );	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE );	  //ʹ��ADC1ͨ��ʱ��
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//PA1 ��Ϊģ��ͨ����������                         
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
	}
	{
		ADC_InitTypeDef ADC_InitStructure; 
		
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
		ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1�����ڶ���ģʽ
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڷ�����ת��ģʽ
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//ת���ɶ�ʱ��2��ͨ��2������ֻ����������ʱ���Դ�����
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
		ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
		ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

		ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1		
		
		ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
		 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5 );//ADC1ͨ��1,����ʱ��Ϊ239.5����	 
		 
		ADC_ResetCalibration(ADC1);//��λ��׼�Ĵ���
		 
		while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
		
		ADC_StartCalibration(ADC1);	 //����ADУ׼
	 
		while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
	 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������
		
		ADC_DMACmd(ADC1, ENABLE);	//ADC��DMA����ʹ��
	}
}

//���ADCֵ
u16 get_adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

///******************************************************************
//��������:bsp_timer2_init(u16 arr,u16 psc,u16 pulse)
//��������:��ʱ��3��PWM���ģʽ��ʼ������
//����˵��: arr����װ��ֵ
//		   psc��Ԥ��Ƶֵ
//         pulse:ռ�ձ�ֵ
//��    ע:ͨ��TIM2-CH2��PWM�������ADC����
//*******************************************************************/ 	
void bsp_timer2_init(u16 arr,u16 psc,u16 pulse)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
	{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		//��ʼ��TIM2
		TIM_TimeBaseStructure.TIM_Period = arr-1;   //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
		TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	}
	{
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		//��ʼ��TIM2 Channel2 PWMģʽ	 
		TIM_OCInitStructure.TIM_OCMode =      TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
		TIM_OCInitStructure.TIM_OCPolarity =  TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
		TIM_OCInitStructure.TIM_Pulse=        pulse;
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2

		TIM_CtrlPWMOutputs(TIM2, ENABLE);//ʹ��PWM���
	}
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
}

volatile uint16_t ADC_ConvertedValue[1024];			    //ADC����������

void adc_dma_init() // cparΪ�����ַ��cmarΪ�ڴ��ַ��cndtrΪ���˵����ݸ�������Ӧ�����鳤�ȣ�
{
    DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	
    DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32) & ADC1->DR;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�//
	DMA_InitStructure.DMA_BufferSize = 1024;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //ADC1ƥ��DMAͨ��1
	
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	//ʹ��DMA�����ж�	
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	DMA_Cmd(DMA1_Channel1,ENABLE);//ʹ��DMAͨ��
}
