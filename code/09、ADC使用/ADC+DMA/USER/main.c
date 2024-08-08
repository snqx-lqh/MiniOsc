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

#include "osc_ui.h" 

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
	bsp_timer2_init(720,1,2);
	adc_dma_init();
	
	osc_ui_init();
	
	while(1)
	{
		while(adc_flag==0);     //���ɼ���ɺ�����ѭ��
		
		//�����ݴ���		
		osc_ui_loop();
		
		adc_flag=0;                        //����־λ��0
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
 