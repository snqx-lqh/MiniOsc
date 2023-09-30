#include "stm32f10x.h"

#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_exti.h"

//外部中断实验
   
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	bsp_exti_init();
	
	while(1)
	{

	}
}


//按键输入检测实验
/*    
int main(void)
{	
	bsp_led_init();
	bsp_key_init();
	
	while(1)
	{
		if(get_key_wkup_value() == 1)
		{
			led1_on();
		}else
		{
			led1_off();
		}
		
		if(get_key_0_value() == 0)
		{
			led0_on();
		}else
		{
			led0_off();
		}
	}
}
*/
