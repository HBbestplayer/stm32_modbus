#ifndef _CHOOSE_H
#define _CHOOSE_H

//modbus错误码
typedef enum{
	ERR1 = 1,	//非法功能码
	ERR2 = 2,	//非法数据地址
	ERR3 = 3,	//非法数据值,长度不对
	RRR4 = 4,	//从机设备故障
	RRR5 = 8,	//校验错误	
}Error_Code;



void Modbus_Num3 (void);
void Modbus_Num6(unsigned char *ptr, unsigned char len);
unsigned char Modbus_data_check(unsigned char *tcp_recbuf,unsigned char len);
void Modbus_Send_Error(unsigned char Cmd, unsigned char Err);
void Modbus_Handle(void);

#endif
