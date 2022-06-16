/******************************************************************************
  * @brief  CHECK_MODBUS：判断接受到的数据是否正确，并提取表示浓度的数据
  * @param  
       ptr:接收到的在缓存中的modbus指令，len:ptr的长度
  * @retval 
 ******************************************************************************/
#include "handle.h"
#include "crc.h"
#include <stdio.h>
#include "rs485.h"
#include "usart.h"


extern rtu_modbus modbus;
extern tcp_modbus tcpbus;
u8 handle_flag;                   //处理标志位，用于等待，没有处理证明没接受完，继续等待接收



void Check_Modbus(u8  *ptr, u8 len)
{   
	u8 temp_after[2];
	u8 temp_before[12];
	u16 crc;
	u8 crc_8[2];
	u8 temp_0;
	u8 temp_1;
	u8 data_length;
	u8 crc_i;
	u8 data_i;
	u8 data_temp[10];
	

//按传感器设定好
	modbus.sendaddr = 0x02;    //要发送的传感器地址，用于判断是否返回正确
	u8 mode= 0x03;

	  if(modbus.reflag==0)       //没有收完modbus的数据包
	  {
	       return ;                //没有收到处理指令，return会跳出函数，然后继续等待下一条数据
	  }
    //收到返回的数据包，先判断返回的地址码，功能码是否有误。
    if (ptr[0] == modbus.sendaddr)
    {
        if (ptr[1] == mode)
        {
            //再判断crc校验码是不是正确,temp_after临时存放后两位数据,temp_before存原指针（除去后两位）算crc
           
            
            for (crc_i = 0; crc_i < len; crc_i++)
            {   
							if(crc_i<len-2)
								{
							temp_before[crc_i] = ptr[crc_i];
						    }
								else
									{
							temp_after[crc_i-(len-2)] = ptr[crc_i];	
								}
            }
            
            crc = DO_CRC_TABLE(temp_before, (len - 2));
            Variable16ToArray(crc_8, crc);
            temp_0 = crc_8[0];
            temp_1 = crc_8[1];
            if ((temp_0 == temp_after[1]) && (temp_1 == temp_after[0]))
            { 
//                printf("传输无误\r\n");
                //获取有用的数据
                //先获取数据位第三位，看数据长度
                data_length = ptr[2];
//                u8 data_temp[data_length];
                for (data_i = 0; data_i < data_length; data_i++)
                {
                    data_temp[data_i] = ptr[data_i + 3];
                }
                switch (data_length)
                {
                case 2:
                {
                    float co2_temp = ((float)(16 * 16 * ((uint32_t)data_temp[0]) + (uint32_t)data_temp[1])) / 10;
									  
//                    printf("湿度浓度为：%f\r\n", co2_temp);
                    break;
                }
                case 4:
                {
                    float temperature_temp = ((float)(16 * 16 * ((uint32_t)data_temp[0]) + (uint32_t)data_temp[1])) / 10;
                    float humidity_temp = ((float)(16 * 16 * ((uint32_t)data_temp[2]) + (uint32_t)data_temp[3])) / 10;
									  
//                    printf("湿度为：%f，温度为：%f\r\n", temperature_temp, humidity_temp);
                    break;
                }
                default:
                    printf("error\r\n");
                }
            }
            else
            {
                printf("crc校验不通过，传输有误\r\n");
            }
        }
        else
        {
            printf("功能码不一致，传输有误\r\n");
        }
    }
    else
    {
        printf("地址码不一致，传输有误\r\n");
    }
		tcpbus.temprecount=modbus.recount;             //先将长度存到tcpbus中
		modbus.recount=0;                          //再清除缓存计数
    modbus.reflag=0;	                         //接收标志置0，准备下次接收
		handle_flag=1;
		
}


