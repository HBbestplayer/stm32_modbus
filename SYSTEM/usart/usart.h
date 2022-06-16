#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_MAXLEN  			100  	//�����������ֽ��� 200
#define USART_REC_MINLEN         7
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define SENDLENG       64
#define REVCLENG       64


//ģʽ����
#define DTU_RX_EN		PAout(12)	//485ģʽ���տ��ƣ���RE��0ʱ��Ч
#define DTU_TX_EN		PAout(13)	//485ģʽ���Ϳ��ƣ���DE��1ʱ��Ч
	  	
//extern u8  USART_RX_BUF[64]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
//extern u16 USART_RX_STA;         		//����״̬���	


typedef struct
{
 volatile unsigned char  threshold[2];   //��ֵ�洢
 unsigned char  rcbuf[REVCLENG];     //MODBUS���ջ�����
 unsigned char  sendbuf[SENDLENG];   //MODbus���ͻ�����	
 unsigned char  myaddr;         //�����豸�ĵ�ַ
 volatile unsigned char  recount;        //���յ���ָ��ĳ���
 volatile unsigned char  temprecount;    //���洫�����������ݵĳ���
 volatile unsigned char  recflag;        //���ձ�־λ 
 volatile unsigned char  timerun;
 volatile unsigned char  timeout; 
	
}tcp_modbus;

extern u8 Cmd_code;
//extern u8 tcp_case;


//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void Todtu_Send_Data(u8 *buf,u8 len);
#endif


