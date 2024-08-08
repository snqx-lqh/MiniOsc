#include "stm32f10x.h"

#include "bsp_led.h"

int main(void)
{	
	bsp_led_init();
	
	led0_off();
	led1_on();
	
	while(1)
	{

	}
}

