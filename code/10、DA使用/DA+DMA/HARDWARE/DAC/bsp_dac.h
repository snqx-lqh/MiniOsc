#ifndef _BSP_DAC_H
#define _BSP_DAC_H

#include "stm32f10x.h"

void dac_all_init(u16 arr,u16 psc);
void bsp_dac_init(void);
void bsp_timer4_init(u16 arr,u16 psc);
void dac_dma_init() ;

#endif
