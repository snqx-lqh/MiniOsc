#include "osc_ui.h"

#include "bsp_adc.h"
#include "bsp_exti.h"

int16_t show_buff[240]={0};

int16_t DIVX  = 200;   //横着一格代表的us数
int16_t DIVY  = 330;   //竖着一格代表的mv数
int16_t VPP   = 0;     //峰峰值
int16_t AVE   = 0;     //平均值
int16_t f_set = 1000;  //波形频率设置
int16_t MAX=0;         //最大值
int16_t MIN=0;         //最小值
int16_t OFFSET = 1650; //偏移量
int32_t OSR    = 100000; //采样频率
int16_t freq   = 0;     //实际频率检测值


/*
划线函数，将Y轴坐标进行翻转变换
*/
void lcd_drawline(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,240-y1,x2,240-y2);
}

/*
显示示波器界面所需的参数名称
*/
void show_const_arg()
{
	LCD_ShowString(0,0,100,16,16,     "f_set:");		
	LCD_ShowString(60,0,100,16,16,    "MAX:");		
	LCD_ShowString(100,0,100,16,16,   "MIN:");
	LCD_ShowString(140,0,100,16,16,   "OSR:(Hz)");
	LCD_ShowString(220,0,100,16,16,   "md:");
	
	LCD_ShowString(250,0,80,16,16,    "DIVX(us):");
	LCD_ShowString(250,40,80,16,16,   "DIVY(mv):");
	LCD_ShowString(250,80,80,16,16,   "VPP:");
	LCD_ShowString(250,120,80,16,16,  "AVE:");
	LCD_ShowString(250,160,80,16,16,  "f(Hz):");
	LCD_ShowString(241,200,80,16,16,  "OFFSET(mv):");			
}

/*
画示波器的框框，和分度线，以及显示各个参数的数值
*/
void show_ui()
{
	int16_t i;
	
	POINT_COLOR = WHITE;
	//画框框
	lcd_drawline(0,0,  0,200);
	lcd_drawline(0,0,  240,0);
	lcd_drawline(0,200,240,200);
	lcd_drawline(240,0,240,200);
	lcd_drawline(120,0,120,200);
	lcd_drawline(0,100,240,100);
	
	//画分度线
	for(i=0;i<12;i++)
	{
		lcd_drawline(20+i*20,100,20+i*20,105);
	}
	for(i=0;i<10;i++)
	{
		lcd_drawline(120,i*20,125,i*20);
	}
	
	POINT_COLOR = BLUE;
	//显示各个参数数值
	LCD_ShowNum(0,20,   f_set,  5,16);
	//LCD_ShowNum(60,20, MAX,    4,16);
	LCD_ShowxNum(60,20, MAX,    4,16,0X80);
	LCD_ShowNum(100,20, MIN,    4,16);
	LCD_ShowNum(140,20, OSR,    6,16);
	LCD_ShowNum(220,20, osc_mode,  2,16);
	LCD_ShowNum(250,20, DIVX,   4,16);
	LCD_ShowNum(250,60, DIVY,   4,16);
	LCD_ShowNum(250,100,VPP,    4,16);
	LCD_ShowNum(250,140,AVE,    4,16);
	LCD_ShowNum(250,180,freq,   5,16);
	LCD_ShowNum(250,220,OFFSET, 4,16);
	 
	POINT_COLOR = YELLOW;
	
}

/*
显示波形
*/
extern int16_t adcx[];

void show_wave()
{
	int16_t i;
	for(i=0;i<239;i++)
	{
		lcd_drawline(i,adcx[i],i+1,adcx[i+1]);
		show_buff[i] = adcx[i];		
	}
	show_buff[239] =  adcx[239];
}

/*
清除上一次波形
原理为先使用白线将上一次所画的线再画一遍，达到清除作用
然后再将画笔转换画波形的颜色
*/
void show_wave_clr()
{
	int16_t i;
	POINT_COLOR = BLACK;
	for(i=0;i<239;i++)
	{
		lcd_drawline(i,show_buff[i],i+1,show_buff[i+1]);
	}
	POINT_COLOR = YELLOW;
}

void osc_ui_init()
{
	LCD_Init();
	POINT_COLOR = RED;
	LCD_Display_Dir(1);  //将显示屏横屏
	LCD_Clear(BLACK);
	show_const_arg();
	show_ui();
	
}

void osc_ui_loop()
{
	show_wave_clr();
	show_ui();
	show_wave();
}