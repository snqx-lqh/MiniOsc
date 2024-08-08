#ifndef _BSP_DAC_H
#define _BSP_DAC_H

#include "stm32f10x.h"

extern int16_t DAC_Value[];
extern int16_t DAC_Value1[];
extern int16_t DAC_Value_BUFF1[];                 
extern int16_t DAC_Value_BUFF2[];
extern int16_t da_change;
extern float DA_VALUE_RATE;

void dac_all_init(u16 arr,u16 psc);
void bsp_dac_init(void);
void bsp_timer4_init(u16 arr,u16 psc);
void dac_dma_init(void) ;
void da_change_wave(void);

#endif
