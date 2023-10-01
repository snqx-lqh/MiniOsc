#ifndef _OSC_UI_H
#define _OSC_UI_H

#include "lcd.h"

extern int16_t DIVX;
extern int16_t DIVY;
extern int16_t VPP;
extern int16_t AVE;
extern int16_t f_set ;
extern int16_t MAX;
extern int16_t MIN;
extern int16_t OFFSET;
extern int32_t OSR;
extern int16_t freq;

void osc_ui_init(void);
void osc_ui_loop(void);

#endif
