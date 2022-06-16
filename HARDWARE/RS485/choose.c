#include "choose.h"
#include "crc.h"
#include <stdio.h>
#include "rs485.h"
#include "usart.h"
#include <string.h>

extern tcp_modbus tcpbus;
extern rtu_modbus modbus;



void Modbus_Num3 (void)          //������������
{

	u8 i;
	tcpbus.recount=tcpbus.temprecount;        //�����ճ��Ȼ��ɻ���ĳ��ȣ����ں���ķ���
//	printf("ԭָ��ȣ�%d\r\n",tcpbus.recount);
	//������recbuf�еĴ��������ݴ���sendbuf������
	for(i=0;i<tcpbus.recount;i++)
	{
		tcpbus.sendbuf[i]=modbus.recbuf[i]; 
//		printf("���������ݣ�%02x\r\n",tcpbus.sendbuf[i]);
	}
	

}


u32 threshold;      //��ֵ

void Modbus_Num6(u8 *ptr, u8 len)         //����ֵ
{
  for(u8 i=0;i<len;i++)  
	{
		tcpbus.sendbuf[i]=ptr[i];                 //���ճɹ���Դ�뷵��
		if(i==4||i==5)
			{
				tcpbus.threshold[i-5]=ptr[i];
			}
	}
	threshold = (u32)tcpbus.threshold[0]+(u32)tcpbus.threshold[1]*256;
//	printf("��ֵΪ��%d\r\n",threshold);

}



u8 Modbus_data_check(u8 *tcp_recbuf,u8 len)      //�жϴ���1���յ�������
{  
	tcpbus.myaddr=0x10;
	u16 pcrc =DO_CRC_TABLE(tcp_recbuf, len-2);
	if(len>6)
	{	
		if(pcrc == ((u16)tcp_recbuf[len-1]<<8|(u16)tcp_recbuf[len-2]))  //У��ɹ�
		{
			if(tcp_recbuf[0]==tcpbus.myaddr)  //��ַ��ȷ
			{
				switch(tcp_recbuf[1])   //�����ؼ��ɵ�һ��0Ϊ�ɹ�����ԭ�룬��������
				{
					
					case 0x03:
						Modbus_Num3 ();
						break;
					case 0x06:
						Modbus_Num6(tcp_recbuf,len);
						break;
					default:
						return 3;   //�Ƿ�������
				}
				return 0;   //�ɹ�ִ�еķ�����,���ǽ�ԭ�뷵��
			}
			 return 1;   //��ַ����ķ�����
		}
		
		 return 2;    //crc����ķ�����
	}
	  return 4;    //����������
}

void Modbus_Send_Error(u8 Cmd, u8 Err)    //������
{ 
	
	//����Ϊ����ģʽ
	DTU_RX_EN,DTU_TX_EN	=1;
	
	u16 temp_crc = 0;	

	tcpbus.sendbuf[0] = tcpbus.myaddr;
	tcpbus.sendbuf[1] = Cmd;			 //������
	tcpbus.sendbuf[2] = Err;
	temp_crc = DO_CRC_TABLE(tcpbus.sendbuf,3);
	tcpbus.sendbuf[3] = (u8)((temp_crc>>8)&0x00ff);
	tcpbus.sendbuf[4] = (u8)(temp_crc&0x00ff);
	for (u8 i=0;i<5;i++)
	{ 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData( USART1,tcpbus.sendbuf[i]);
//		printf("����1���ͣ�%02x\r\n,",tcpbus.sendbuf[i]);
	}
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	//���ûؽ���ģʽ
	DTU_RX_EN,DTU_TX_EN	=0;
	memset(tcpbus.sendbuf,0,SENDLENG);   //�Է��ͻ�����������
}



void Modbus_Handle(void)    
{
	
		if(tcpbus.recflag==0)        //û�����겻����
		{
			return;
		}
		//�ɹ��򷵻�ԭmodbusָ��
		if(Modbus_data_check(tcpbus.rcbuf,tcpbus.recount)==0)  
		{
				Todtu_Send_Data(tcpbus.sendbuf,tcpbus.recount);
				
		}	
		//���򷵻ش�����
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
		tcpbus.recount=0;        //�������
		tcpbus.recflag=0;        //��־λ��λ
//		memset(tcpbus.rcbuf,0,SENDLENG);   //��ս��ջ��������ȴ���һ�εĽ���					
					

}


