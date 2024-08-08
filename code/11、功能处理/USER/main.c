#include "stm32f10x.h"

#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_exti.h"
#include "bsp_timer.h"
#include "bsp_usart.h"
#include "bsp_delay.h" 
#include "bsp_sys.h"
#include "lcd.h" 
#include "bsp_adc.h"
#include "bsp_dac.h"

#include "osc_ui.h" 

#include "math.h"
#include "stm32_dsp.h"
#include "table_fft.h"

uint8_t adc_flag = 0;

#define NPT 1024 //��������
#define PI2 6.28318530717959

int long fftin [NPT];//FFT����
int long fftout[NPT];//FFT���
u32 FFT_Mag[NPT/2]={0};//��Ƶ����
u16 magout[NPT];//ģ�����Ҳ����������

int16_t adcx[1024] = {0};

void GetPowerMag();

int main(void)
{	
	int i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	bsp_exti_init();
	bsp_timer_init(5000,72);
	bsp_usart1_init(115200);
	delay_init();
	bsp_adc_init();
	bsp_timer2_init(720,1,2);
	adc_dma_init();
	dac_all_init(720,1);
	osc_ui_init();
	
	while(1)
	{
		while(adc_flag==0);     //���ɼ���ɺ�����ѭ��
		
		for(i=0;i<NPT;i++)
		{
			adcx[i] = ADC_ConvertedValue[i];
			fftin[i] = 0;
			fftin[i] = adcx[i] << 16;
		}
		GetPowerMag();    //����Ҷ�任
		
		MAX = adcx[0];               
		MIN = adcx[0];
		
		for(i=0;i<NPT;i++)      //����������Сֵ
		{		
			if(MAX <= adcx[i])
			{
				MAX = adcx[i];
			}
			if(MIN >= adcx[i])
			{
				MIN = adcx[i];
			}
		}
		
		if(wave_mode==0)//���Ҳ�AVE
		{
//			AVE = (int16_t)((((((float)(MAX-2048)/1.57)+2048)*3300))/4095);//���Ҳ���ֵ
			AVE = (int16_t)((float)(MAX*3300)/1.57/4095);
		}else if(wave_mode==1)//����AVE
		{
//			AVE = (int16_t)((float)((((MAX - 2048)/2)+2048)*3300)/4095);//���ǲ���
			AVE = (int16_t)(((float)(MAX/2)*3300)/4095);
		}
		
		MAX = MAX*3300/4095;
		MIN = MIN*3300/4095;
		
		/*
		����ʵ��ֵ�������������
		*/
		VPP = MAX - MIN;                   //���ֵ
		
		/*
		��ÿһ�εĲ���ƫ�õ�������ȥ
		��Ϊ���������½�Ϊ��������ԭ��,��ʹ�ֵΪ�㣬��ߴ�ֵΪ4095������ÿһ�θı��ֵ�����ζ���������
		*/
		for(i=0;i<NPT;i++)      
		{
			adcx[i] = (adcx[i]+(2048-(4096*DA_VALUE_RATE)*0.5));
		}
		OFFSET = (4096*DA_VALUE_RATE)*0.5*3300/4096;
		
		/*
		��ÿ��ֵ�ȱ�����С��y�����ֻ��200�����ֵȴ��4095��������Ҫ�ȱ�����С
		*/
		for(i=0;i<NPT;i++)
		{
			if(divy_mode==0)
				adcx[i] = adcx[i]* 200/4096+0;
			else if(divy_mode==1)
				adcx[i] = adcx[i]* 100/4096+50;
			else if(divy_mode==2)
				adcx[i] = adcx[i]* 50/4096+75;
		}
		if(divy_mode==0)
			DIVY = 330;
		else if(divy_mode==1)
			DIVY = 660;
		else if(divy_mode==2)
			DIVY = 1320;
		
		if(stop_mode == 0)
			osc_ui_loop();
		
		adc_flag=0;                        //����־λ��0
		da_change_wave();                  ////DA������иı䣬�ı��������
		delay_ms(500);                     //1000msˢ��һ�β���
		DMA_Cmd(DMA1_Channel1, ENABLE);    //ʹ��AD�ɼ�
	}
}

/*
DMA1�ж�AD�ɼ��жϣ�ÿ�ɼ���1024��ֵ������ж�
*/
void DMA1_Channel1_IRQHandler(void) 
{
	if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
	{                   
		adc_flag=1;                        //��AD�ɼ���ɱ�־λ��1
		DMA_Cmd(DMA1_Channel1, DISABLE);   //ʧ��AD�ɼ�ͨ��     
	}
	DMA_ClearITPendingBit(DMA1_IT_TC1);         
}
 
void GetPowerMag()
{
	float X,Y,Mag,magmax;//ʵ�����鲿����Ƶ�ʷ�ֵ������ֵ
    u16 i;
	u16 temp=0;//��ֵ����Ƶ�ʳɷ�
	
	//������cr4_fft_1024_stm32
	cr4_fft_1024_stm32(fftout, fftin, NPT);	
	//fftinΪ����Ҷ�����������飬ffoutΪ����Ҷ�����������
	
    for(i=1; i<NPT/2; i++)
    {
		X = (fftout[i] << 16) >> 16;
		Y = (fftout[i] >> 16);
		
		Mag = sqrt(X * X + Y * Y); 
		FFT_Mag[i]=Mag;//���뻺�棬�����������
		//��ȡ���Ƶ�ʷ��������ֵ
		if(Mag > magmax)
		{
			magmax = Mag;
			temp = i;
		}
    }
    freq=((float)OSR*temp/1024);
}
