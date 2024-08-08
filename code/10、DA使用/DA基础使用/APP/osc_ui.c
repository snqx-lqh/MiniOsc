#include "osc_ui.h"

#include "bsp_adc.h"

int16_t show_buff[240]={0};

/*
���ߺ�������Y��������з�ת�任
*/
void lcd_drawline(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,240-y1,x2,240-y2);
}

/*
��ʾʾ������������Ĳ�������
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
��ʾ�����Ŀ�򣬺ͷֶ��ߣ��Լ���ʾ������������ֵ
*/
void show_ui()
{
	int16_t i;
	
	POINT_COLOR = WHITE;
	//�����
	lcd_drawline(0,0,  0,200);
	lcd_drawline(0,0,  240,0);
	lcd_drawline(0,200,240,200);
	lcd_drawline(240,0,240,200);
	lcd_drawline(120,0,120,200);
	lcd_drawline(0,100,240,100);
	
	//���ֶ���
	for(i=0;i<12;i++)
	{
		lcd_drawline(20+i*20,100,20+i*20,105);
	}
	for(i=0;i<10;i++)
	{
		lcd_drawline(120,i*20,125,i*20);
	}
	
	POINT_COLOR = BLUE;
	//��ʾ����������ֵ
	LCD_ShowNum(0,20,   10,  5,16);
	LCD_ShowxNum(60,20, 20,    4,16,0X80);
	LCD_ShowNum(100,20, 30,    4,16);
	LCD_ShowNum(140,20, 40,    6,16);
	LCD_ShowNum(220,20, 50,    2,16);
	LCD_ShowNum(250,20, 60,    4,16);
	LCD_ShowNum(250,60, 70,    4,16);
	LCD_ShowNum(250,100,80,    4,16);
	LCD_ShowNum(250,140,90,    4,16);
	LCD_ShowNum(250,180,100,   5,16);
	LCD_ShowNum(250,220,110,   4,16);
	 
	POINT_COLOR = YELLOW;
	
}

/*
��ʾ����
*/
void show_wave()
{
	int16_t i;
	for(i=0;i<239;i++)
	{
		lcd_drawline(i,ADC_ConvertedValue[i]*200/4096,i+1,ADC_ConvertedValue[i+1]*200/4096);
		show_buff[i] = ADC_ConvertedValue[i]*200/4096;		
	}
	show_buff[239] =  ADC_ConvertedValue[239]*200/4096;
}

/*
�����һ�β���
ԭ��Ϊ��ʹ�ð��߽���һ�����������ٻ�һ�飬�ﵽ�������
Ȼ���ٽ�����ת�������ε���ɫ
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
	LCD_Display_Dir(1);  //����ʾ������
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