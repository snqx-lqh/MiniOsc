#ifndef _BSP_EXTI_H
#define _BSP_EXTI_H

#include "stm32f10x.h"

extern int16_t osc_mode;
extern int16_t wave_mode;
extern int16_t stop_mode;
extern int16_t divy_mode;

void bsp_exti_init(void);

#endif

