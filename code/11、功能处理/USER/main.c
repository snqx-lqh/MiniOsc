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

#define NPT 1024 //采样次数
#define PI2 6.28318530717959

int long fftin [NPT];//FFT输入
int long fftout[NPT];//FFT输出
u32 FFT_Mag[NPT/2]={0};//幅频特性
u16 magout[NPT];//模拟正弦波输出缓存区

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
		while(adc_flag==0);     //当采集完成后跳出循环
		
		for(i=0;i<NPT;i++)
		{
			adcx[i] = ADC_ConvertedValue[i];
			fftin[i] = 0;
			fftin[i] = adcx[i] << 16;
		}
		GetPowerMag();    //傅里叶变换
		
		MAX = adcx[0];               
		MIN = adcx[0];
		
		for(i=0;i<NPT;i++)      //计算出最大最小值
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
		
		if(wave_mode==0)//正弦波AVE
		{
//			AVE = (int16_t)((((((float)(MAX-2048)/1.57)+2048)*3300))/4095);//正弦波均值
			AVE = (int16_t)((float)(MAX*3300)/1.57/4095);
		}else if(wave_mode==1)//方波AVE
		{
//			AVE = (int16_t)((float)((((MAX - 2048)/2)+2048)*3300)/4095);//三角波均
			AVE = (int16_t)(((float)(MAX/2)*3300)/4095);
		}
		
		MAX = MAX*3300/4095;
		MIN = MIN*3300/4095;
		
		/*
		根据实际值，计算各个参数
		*/
		VPP = MAX - MIN;                   //峰峰值
		
		/*
		将每一次的波形偏置到横线上去
		因为我是以左下角为波形坐标原点,最低处值为零，最高处值为4095，所以每一次改变幅值，波形都会往下走
		*/
		for(i=0;i<NPT;i++)      
		{
			adcx[i] = (adcx[i]+(2048-(4096*DA_VALUE_RATE)*0.5));
		}
		OFFSET = (4096*DA_VALUE_RATE)*0.5*3300/4096;
		
		/*
		将每个值等比例缩小，y轴格子只有200格，最大值却是4095，所以需要等比例缩小
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
		
		adc_flag=0;                        //将标志位置0
		da_change_wave();                  ////DA输出若有改变，改变输出缓存
		delay_ms(500);                     //1000ms刷新一次波形
		DMA_Cmd(DMA1_Channel1, ENABLE);    //使能AD采集
	}
}

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
 
void GetPowerMag()
{
	float X,Y,Mag,magmax;//实部，虚部，各频率幅值，最大幅值
    u16 i;
	u16 temp=0;//幅值最大的频率成分
	
	//调用自cr4_fft_1024_stm32
	cr4_fft_1024_stm32(fftout, fftin, NPT);	
	//fftin为傅里叶输入序列数组，ffout为傅里叶输出序列数组
	
    for(i=1; i<NPT/2; i++)
    {
		X = (fftout[i] << 16) >> 16;
		Y = (fftout[i] >> 16);
		
		Mag = sqrt(X * X + Y * Y); 
		FFT_Mag[i]=Mag;//存入缓存，用于输出查验
		//获取最大频率分量及其幅值
		if(Mag > magmax)
		{
			magmax = Mag;
			temp = i;
		}
    }
    freq=((float)OSR*temp/1024);
}
