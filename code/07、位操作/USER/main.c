#include "stm32f10x.h"

#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_exti.h"
#include "bsp_timer.h"
#include "bsp_usart.h"
#include "bsp_delay.h" 
#include "bsp_sys.h"
 
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	//bsp_exti_init();
	bsp_timer_init(5000,72);
	bsp_usart1_init(115200);
	
	delay_init();
	
	printf("hello world\r\n");
	while(1)
	{
		if(PAin(0) == 1)
		{
			PAout(8) = 0;
		}else
		{
			PAout(8) = 1;
		}
		
		if(PAin(15) == 0)
		{
			PDout(2) = 0;
		}else
		{
			PDout(2) = 1;
		}
	}
}

