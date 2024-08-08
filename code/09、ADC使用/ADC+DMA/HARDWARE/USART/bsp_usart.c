#include "bsp_usart.h"


int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0){}//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}

void bsp_usart1_init(uint32_t bound)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//���ų�ʼ��
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		//USART1_TX   GPIOA.9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//USART1_RX	  GPIOA.10��ʼ��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	}
	//���ڳ�ʼ��
	{
		USART_InitTypeDef USART_InitStructure;
		
		USART_InitStructure.USART_BaudRate   = bound;//���ڲ�����
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits   = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity     = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

		USART_Init(USART1, &USART_InitStructure);
	}
	//NVIC��ʼ��
	{
		NVIC_InitTypeDef NVIC_InitStructure;
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);
	}
	//���������жϺʹ����ж�ʹ��
	{
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
		USART_Cmd(USART1, ENABLE);
	}
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		USART_SendData(USART1,Res);
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
} 