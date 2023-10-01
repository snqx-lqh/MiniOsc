#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "stm32f10x.h"

void bsp_key_init(void);
uint8_t get_key_wkup_value(void);
uint8_t get_key_0_value(void);
uint8_t get_key_1_value(void);


#endif
