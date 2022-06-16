#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_MAXLEN  			100  	//定义最大接收字节数 200
#define USART_REC_MINLEN         7
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define SENDLENG       64
#define REVCLENG       64


//模式控制
#define DTU_RX_EN		PAout(12)	//485模式接收控制，接RE，0时有效
#define DTU_TX_EN		PAout(13)	//485模式发送控制，接DE，1时有效
	  	
//extern u8  USART_RX_BUF[64]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//extern u16 USART_RX_STA;         		//接收状态标记	


typedef struct
{
 volatile unsigned char  threshold[2];   //阈值存储
 unsigned char  rcbuf[REVCLENG];     //MODBUS接收缓冲区
 unsigned char  sendbuf[SENDLENG];   //MODbus发送缓冲区	
 unsigned char  myaddr;         //本机设备的地址
 volatile unsigned char  recount;        //接收到的指令的长度
 volatile unsigned char  temprecount;    //缓存传感器接收数据的长度
 volatile unsigned char  recflag;        //接收标志位 
 volatile unsigned char  timerun;
 volatile unsigned char  timeout; 
	
}tcp_modbus;

extern u8 Cmd_code;
//extern u8 tcp_case;


//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void Todtu_Send_Data(u8 *buf,u8 len);
#endif


