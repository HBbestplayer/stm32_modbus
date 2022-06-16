#include "sys.h"
#include "usart.h"	
#include "delay.h"
 
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数，修改到串口3 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0)
		;//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
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
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	

  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	
	//还要选个管脚作为控制485收发的，同时要定义该管脚什么时候收和发，默认是收
	//USART1_控收   GPIOA.12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PA12端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART1_控发   GPIOA.13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //PA12端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,ENABLE);//复位串口2
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,DISABLE);//停止复位

  //Usart1 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
	
	
	//默认为接收模式
	DTU_RX_EN,DTU_TX_EN	=0;
	
	

}

u8 Cmd_code;
tcp_modbus tcpbus;


void USART1_IRQHandler(void)                	//tcp
	{
	u8 Res;
 
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收dtu的数据，即写阈值)
		{    
				USART_ClearITPendingBit(USART1,USART_IT_RXNE);
			  
//			  tcp_case=1;                                 //进入tcp中断，该定时器标志位置1
			
				Res =USART_ReceiveData(USART1);	//读取接收到的数据
				tcpbus.rcbuf[tcpbus.recount]=Res ;
				if (tcpbus.recount == 1)                //有数据接收就开启定时器          
			  {    
					 tcpbus.timerun = 1; 
					 Cmd_code = tcpbus.rcbuf[1];
			  }
//				printf("串口1接收：%02x\r\n",Res);
				tcpbus.recount++;
			
				tcpbus.timeout=0;
			

				
					 

	//					if(tcpbus.recount>(USART_REC_MAXLEN-1)&&tcpbus.recount<(USART_REC_MINLEN-1))tcpbus.recount=0;//接收数据错误,重新开始接收	  

     } 

} 
#endif	

void Todtu_Send_Data(u8 *buf,u8 len)
{  
	 //设置为发送模式
	 DTU_RX_EN,DTU_TX_EN	=1;
//	 delay_us(10);
	
	 for(u8 t=0;t<len;t++)		//循环发送数据
	{	
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1,buf[t]);
		
	}	 
	
   while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		;	
//	 printf("发送上位机成功\r\n");
	 //设置回接收模式
	 DTU_RX_EN,DTU_TX_EN	=0;
	 delay_us(10);
}

