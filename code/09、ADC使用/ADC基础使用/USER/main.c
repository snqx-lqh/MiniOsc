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

int main(void)
{	
	uint16_t adc_value = 0;
	float    voltage   = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_led_init();
	bsp_key_init();
	bsp_exti_init();
	bsp_timer_init(5000,72);
	bsp_usart1_init(115200);
	delay_init();
	bsp_adc_init();
	
	osc_ui_init();
	
	while(1)
	{
		adc_value = get_adc(1);
		printf("adc_value : %d ",adc_value);
		voltage = adc_value * 3.3/4095;
		printf("voltage_value : %f\r\n",voltage);
		delay_ms(500);
	}
}
 