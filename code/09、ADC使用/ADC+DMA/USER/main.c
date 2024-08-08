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
		while(adc_flag==0);     //当采集完成后跳出循环
		
		//做数据处理		
		osc_ui_loop();
		
		adc_flag=0;                        //将标志位置0
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
 