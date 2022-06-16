/******************************************************************************
  * @brief  CHECK_MODBUS���жϽ��ܵ��������Ƿ���ȷ������ȡ��ʾŨ�ȵ�����
  * @param  
       ptr:���յ����ڻ����е�modbusָ�len:ptr�ĳ���
  * @retval 
 ******************************************************************************/
#include "handle.h"
#include "crc.h"
#include <stdio.h>
#include "rs485.h"
#include "usart.h"


extern rtu_modbus modbus;
extern tcp_modbus tcpbus;
u8 handle_flag;                   //�����־λ�����ڵȴ���û�д���֤��û�����꣬�����ȴ�����



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
	

//���������趨��
	modbus.sendaddr = 0x02;    //Ҫ���͵Ĵ�������ַ�������ж��Ƿ񷵻���ȷ
	u8 mode= 0x03;

	  if(modbus.reflag==0)       //û������modbus�����ݰ�
	  {
	       return ;                //û���յ�����ָ�return������������Ȼ������ȴ���һ������
	  }
    //�յ����ص����ݰ������жϷ��صĵ�ַ�룬�������Ƿ�����
    if (ptr[0] == modbus.sendaddr)
    {
        if (ptr[1] == mode)
        {
            //���ж�crcУ�����ǲ�����ȷ,temp_after��ʱ��ź���λ����,temp_before��ԭָ�루��ȥ����λ����crc
           
            
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
//                printf("��������\r\n");
                //��ȡ���õ�����
                //�Ȼ�ȡ����λ����λ�������ݳ���
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
									  
//                    printf("ʪ��Ũ��Ϊ��%f\r\n", co2_temp);
                    break;
                }
                case 4:
                {
                    float temperature_temp = ((float)(16 * 16 * ((uint32_t)data_temp[0]) + (uint32_t)data_temp[1])) / 10;
                    float humidity_temp = ((float)(16 * 16 * ((uint32_t)data_temp[2]) + (uint32_t)data_temp[3])) / 10;
									  
//                    printf("ʪ��Ϊ��%f���¶�Ϊ��%f\r\n", temperature_temp, humidity_temp);
                    break;
                }
                default:
                    printf("error\r\n");
                }
            }
            else
            {
                printf("crcУ�鲻ͨ������������\r\n");
            }
        }
        else
        {
            printf("�����벻һ�£���������\r\n");
        }
    }
    else
    {
        printf("��ַ�벻һ�£���������\r\n");
    }
		tcpbus.temprecount=modbus.recount;             //�Ƚ����ȴ浽tcpbus��
		modbus.recount=0;                          //������������
    modbus.reflag=0;	                         //���ձ�־��0��׼���´ν���
		handle_flag=1;
		
}


