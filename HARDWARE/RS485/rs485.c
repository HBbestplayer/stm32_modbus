#include "sys.h"		    
#include "rs485.h"	 
#include "delay.h"
#include "led.h"
#include <stdio.h>
#include "stm32f10x.h"



rtu_modbus modbus;


#ifdef EN_USART2_RX   	//如果使能了接收


  
void USART2_IRQHandler(void)      //rtu
{
	u8 res;	    
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{	 
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		
//		rtu_case=1;                       //进入rtu中断，该定时器标志位置1
				 
		res =USART_ReceiveData(USART2); 	//读取接收到的数据

		modbus.recbuf[modbus.recount]=res;		//记录接收到的值
//		printf("返回的十六进制数：%d,%02x\r\n",modbus.recount,modbus.recbuf[modbus.recount]);
		modbus.recount++;						//接收数据增加1 

		modbus.timeout = 0;  		//在定时中断过程中，有数据接收，则timeout计数重置
		
		
		if (modbus.recount == 1)                //有数据接收就开启定时器          
		{
				 modbus.timerun = 1; 
		}
//		      if(modbus.recount<( RS485_MINLEN -1))modbus.recount=0;    //接收低于7字节，重新开始接收
	}  
	
} 
#endif										 
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void RS485_Init(u32 bound)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);//使能GPIOA,D时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //PD7端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位
 
	
 #ifdef EN_USART2_RX		  	//如果使能了接收
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
   
  USART_Cmd(USART2, ENABLE);                    //使能串口 

 #endif

  RS485_TX_EN=0;			//默认为接收模式
 
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{   
    
	  RS485_TX_EN=1;			    //设置为发送模式
//	  printf("进入发送\r\n");
  	for(u8 t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
		LED1=!LED1;
	}	 
 
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		;	
	  RS485_TX_EN=0;				//设置为接收模式	
}





















