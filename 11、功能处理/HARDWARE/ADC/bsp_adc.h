#ifndef _BSP_ADC_H
#define _BSP_ADC_H

#include "stm32f10x.h"

void bsp_adc_init(void);
u16 get_adc(u8 ch); 
void bsp_timer2_init(u16 arr,u16 psc,u16 pulse);
void adc_dma_init(void);

extern volatile uint16_t ADC_ConvertedValue[];			    //ADC采样的数据

#endif

