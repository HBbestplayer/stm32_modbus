#ifndef _HANDLE_H
#define _HANDLE_H

#include "stm32f10x.h"

extern u8 handle_flag;

void Check_Modbus(unsigned char  *ptr, unsigned char len);

#endif


