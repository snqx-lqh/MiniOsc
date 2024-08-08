#include "bsp_dac.h"

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
		DAC_InitType.DAC_Trigger=DAC_Trigger_None;	                  //
		DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;          //��ʹ�ò��η���
		DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
		DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	      //DAC1�������ر� BOFF1=1
		DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1

		DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
		DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DAC1
	}
}