#ifndef _RS485_H
#define _RS485_H			 
#include "sys.h"	 								  


#define RS485_MINLEN  7

typedef struct
{
  unsigned char   sendaddr;                //�������豸�ĵ�ַ
  unsigned char   recbuf[20];              //���ջ�����
  volatile unsigned int   timeout;         //���ݶ���ʱ��	
  volatile unsigned char    recount;       //�˿��Ѿ��յ������ݸ���
  volatile unsigned char    timerun;       //��ʱ���Ƿ��ʱ�ı�־
  volatile unsigned char    reflag;        //�յ�һ֡���ݵı�־
  unsigned char   sendbuf[100];            //���ͻ�����	
}rtu_modbus;

//extern unsigned char tcp_case;
//extern unsigned char rtu_case;

//ģʽ����
#define RS485_TX_EN		PDout(7)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define EN_USART2_RX 	1			//0,������;1,����.




void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);


#endif	   
















