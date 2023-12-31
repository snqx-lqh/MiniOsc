#include "bsp_usart.h"


int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}

void bsp_usart1_init(uint32_t bound)
{
	//打开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//引脚初始化
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//USART1_TX   GPIOA.9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART1_RX	  GPIOA.10初始化
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	}
	//串口初始化
	{
		USART_InitTypeDef USART_InitStructure;
		
		USART_InitStructure.USART_BaudRate   = bound;//串口波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits   = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity     = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART1, &USART_InitStructure);
	}
	//NVIC初始化
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);
	}
	//开启串口中断和串口中断使能
	{
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
		USART_Cmd(USART1, ENABLE);
	}
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		USART_SendData(USART1,Res);
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
} 