#ifndef _RS485_H
#define _RS485_H			 
#include "sys.h"	 								  


#define RS485_MINLEN  7

typedef struct
{
  unsigned char   sendaddr;                //传感器设备的地址
  unsigned char   recbuf[20];              //接收缓冲区
  volatile unsigned int   timeout;         //数据断续时间	
  volatile unsigned char    recount;       //端口已经收到的数据个数
  volatile unsigned char    timerun;       //定时器是否计时的标志
  volatile unsigned char    reflag;        //收到一帧数据的标志
  unsigned char   sendbuf[100];            //发送缓冲区	
}rtu_modbus;

//extern unsigned char tcp_case;
//extern unsigned char rtu_case;

//模式控制
#define RS485_TX_EN		PDout(7)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，请不要注释以下宏定义
#define EN_USART2_RX 	1			//0,不接收;1,接收.




void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);


#endif	   
















