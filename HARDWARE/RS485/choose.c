#include "choose.h"
#include "crc.h"
#include <stdio.h>
#include "rs485.h"
#include "usart.h"
#include <string.h>

extern tcp_modbus tcpbus;
extern rtu_modbus modbus;



void Modbus_Num3 (void)          //读传感器数据
{

	u8 i;
	tcpbus.recount=tcpbus.temprecount;        //将接收长度换成缓存的长度，便于后面的发送
//	printf("原指令长度：%d\r\n",tcpbus.recount);
	//将存在recbuf中的传感器数据存入sendbuf缓存中
	for(i=0;i<tcpbus.recount;i++)
	{
		tcpbus.sendbuf[i]=modbus.recbuf[i]; 
//		printf("传感器数据：%02x\r\n",tcpbus.sendbuf[i]);
	}
	

}


u32 threshold;      //阈值

void Modbus_Num6(u8 *ptr, u8 len)         //存阈值
{
  for(u8 i=0;i<len;i++)  
	{
		tcpbus.sendbuf[i]=ptr[i];                 //接收成功将源码返回
		if(i==4||i==5)
			{
				tcpbus.threshold[i-5]=ptr[i];
			}
	}
	threshold = (u32)tcpbus.threshold[0]+(u32)tcpbus.threshold[1]*256;
//	printf("阈值为：%d\r\n",threshold);

}



u8 Modbus_data_check(u8 *tcp_recbuf,u8 len)      //判断串口1接收到的数据
{  
	tcpbus.myaddr=0x10;
	u16 pcrc =DO_CRC_TABLE(tcp_recbuf, len-2);
	if(len>6)
	{	
		if(pcrc == ((u16)tcp_recbuf[len-1]<<8|(u16)tcp_recbuf[len-2]))  //校验成功
		{
			if(tcp_recbuf[0]==tcpbus.myaddr)  //地址正确
			{
				switch(tcp_recbuf[1])   //将返回集成到一起，0为成功返回原码，其他待定
				{
					
					case 0x03:
						Modbus_Num3 ();
						break;
					case 0x06:
						Modbus_Num6(tcp_recbuf,len);
						break;
					default:
						return 3;   //非法功能码
				}
				return 0;   //成功执行的返回码,就是将原码返回
			}
			 return 1;   //地址错误的返回码
		}
		
		 return 2;    //crc错误的返回码
	}
	  return 4;    //长度有问题
}

void Modbus_Send_Error(u8 Cmd, u8 Err)    //功能码
{ 
	
	//设置为发送模式
	DTU_RX_EN,DTU_TX_EN	=1;
	
	u16 temp_crc = 0;	

	tcpbus.sendbuf[0] = tcpbus.myaddr;
	tcpbus.sendbuf[1] = Cmd;			 //功能码
	tcpbus.sendbuf[2] = Err;
	temp_crc = DO_CRC_TABLE(tcpbus.sendbuf,3);
	tcpbus.sendbuf[3] = (u8)((temp_crc>>8)&0x00ff);
	tcpbus.sendbuf[4] = (u8)(temp_crc&0x00ff);
	for (u8 i=0;i<5;i++)
	{ 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData( USART1,tcpbus.sendbuf[i]);
//		printf("串口1发送：%02x\r\n,",tcpbus.sendbuf[i]);
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	//设置回接收模式
	DTU_RX_EN,DTU_TX_EN	=0;
	memset(tcpbus.sendbuf,0,SENDLENG);   //对发送缓存数组清零
}



void Modbus_Handle(void)    
{
	
		if(tcpbus.recflag==0)        //没接受完不运行
		{
			return;
		}
		//成功则返回原modbus指令
		if(Modbus_data_check(tcpbus.rcbuf,tcpbus.recount)==0)  
		{
				Todtu_Send_Data(tcpbus.sendbuf,tcpbus.recount);
				
		}	
		//否则返回错误码
		else
		{
			
				u8 send_err=Modbus_data_check(tcpbus.rcbuf,tcpbus.recount);
				switch(send_err)
				{	
					case 1:
						 Modbus_Send_Error(Cmd_code, (u8)ERR2);
						 break;
					case 2:
						 Modbus_Send_Error(Cmd_code, (u8)RRR5);
						 break;
					case 3:
						 Modbus_Send_Error(Cmd_code, (u8)ERR1);
						 break;
					case 4:
						 Modbus_Send_Error(Cmd_code, (u8)ERR3);
						 break;
						
				}
		}		
		tcpbus.recount=0;        //清除计数
		tcpbus.recflag=0;        //标志位复位
//		memset(tcpbus.rcbuf,0,SENDLENG);   //清空接收缓存区，等待下一次的接收					
					

}


