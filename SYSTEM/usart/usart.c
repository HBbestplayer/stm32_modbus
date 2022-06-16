#include "sys.h"
#include "usart.h"	
#include "delay.h"
 
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc�������޸ĵ�����3 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
		;//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	

  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	
	//��Ҫѡ���ܽ���Ϊ����485�շ��ģ�ͬʱҪ����ùܽ�ʲôʱ���պͷ���Ĭ������
	//USART1_����   GPIOA.12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PA12�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART1_�ط�   GPIOA.13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //PA12�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,ENABLE);//��λ����2
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,DISABLE);//ֹͣ��λ

  //Usart1 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
	
	
	//Ĭ��Ϊ����ģʽ
	DTU_RX_EN,DTU_TX_EN	=0;
	
	

}

u8 Cmd_code;
tcp_modbus tcpbus;


void USART1_IRQHandler(void)                	//tcp
	{
	u8 Res;
 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(����dtu�����ݣ���д��ֵ)
		{    
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			  
//			  tcp_case=1;                                 //����tcp�жϣ��ö�ʱ����־λ��1
			
				Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
				tcpbus.rcbuf[tcpbus.recount]=Res ;
				if (tcpbus.recount == 1)                //�����ݽ��վͿ�����ʱ��          
			  {    
					 tcpbus.timerun = 1; 
					 Cmd_code = tcpbus.rcbuf[1];
			  }
//				printf("����1���գ�%02x\r\n",Res);
				tcpbus.recount++;
			
				tcpbus.timeout=0;
			

				
					 

	//					if(tcpbus.recount>(USART_REC_MAXLEN-1)&&tcpbus.recount<(USART_REC_MINLEN-1))tcpbus.recount=0;//�������ݴ���,���¿�ʼ����	  

     } 

} 
#endif	

void Todtu_Send_Data(u8 *buf,u8 len)
{  
	 //����Ϊ����ģʽ
	 DTU_RX_EN,DTU_TX_EN	=1;
//	 delay_us(10);
	
	 for(u8 t=0;t<len;t++)		//ѭ����������
	{	
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1,buf[t]);
		
	}	 
	
   while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		;	
//	 printf("������λ���ɹ�\r\n");
	 //���ûؽ���ģʽ
	 DTU_RX_EN,DTU_TX_EN	=0;
	 delay_us(10);
}

