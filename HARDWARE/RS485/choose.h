#ifndef _CHOOSE_H
#define _CHOOSE_H

//modbus������
typedef enum{
	ERR1 = 1,	//�Ƿ�������
	ERR2 = 2,	//�Ƿ����ݵ�ַ
	ERR3 = 3,	//�Ƿ�����ֵ,���Ȳ���
	RRR4 = 4,	//�ӻ��豸����
	RRR5 = 8,	//У�����	
}Error_Code;



void Modbus_Num3 (void);
void Modbus_Num6(unsigned char *ptr, unsigned char len);
unsigned char Modbus_data_check(unsigned char *tcp_recbuf,unsigned char len);
void Modbus_Send_Error(unsigned char Cmd, unsigned char Err);
void Modbus_Handle(void);

#endif
