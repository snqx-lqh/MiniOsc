#ifndef _BSP_ADC_H
#define _BSP_ADC_H

#include "stm32f10x.h"

void bsp_adc_init(void);
u16 get_adc(u8 ch); 

#endif

