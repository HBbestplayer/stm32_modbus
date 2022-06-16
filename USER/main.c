//5��29���޸�
//�޸Ķ�ʱ������һ����ʱ���ĳ�������ʱ����TIM3��dtu�˵�ָ�ʱ��TIM4�Դ������˵�ָ�ʱ
//�޸��жϼ�����ȼ���dtu�˵��Ż�����ߣ��ζ��Ǵ������ˣ������������ʱ����
//�޸��˽����жϣ��������ж����printfɾ���������������
//��tcp��֡��ʽ���˸��õع涨����ַ��+������+��ʼ��ַ��2+������2+���ݡ�2+У�����2

//6��3���޸�
//�޸���printf���ض��򵽴���3��ӡ���
//����1��������շ�����λ�����ƶ�Ӧio�ڸߵ͵�ƽ



#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "rs485.h"
#include "handle.h"
#include "timing.h"
#include "choose.h"
#include <string.h>
 				 	
 int main(void)
 {	 
  extern rtu_modbus modbus;
	extern tcp_modbus tcpbus;
	
	u8 wait_time =0;
	u8 getdata_buf[8] = {0x02,0x03,0x00,0x00,0x00,0x01,0x84,0x39};
	
	extern u32 threshold;  //���õ���ֵ
	
	//��ʱ��
	SysClock_Configuration(RCC_PLLSource_HSE_Div1, RCC_CFGR_PLLMULL9);
	TIM3_Init();
	TIM4_Init();
	
	//��ʱ������ʼ��
	delay_init();	    	 	  
	
	//����2-485��ʼ��
	
	RS485_Init(9600);	//��ʼ��RS485
	
	//����1��ʼ��
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	
	
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�	 	 

	
	while(1)
	{ 
    modbus.reflag=0;		  //����ǰ���Ƚ����ձ�־λ��0
//		tcpbus.recflag=0;
		RS485_Send_Data(getdata_buf,8);
		delay_ms(500);

		while(1)
		{
			//��˸LED,��ʾϵͳ��������
		   LED0=!LED0;

			 if(wait_time>2)
				 {
						 wait_time=0;
						 modbus.recount=0;
						 break;                    //�ȴ�15ms,���о����·��Ͳ�ѯ
			   }
				 		 

				 
				 
				//�������˵Ĵ���
				Check_Modbus(modbus.recbuf, modbus.recount);  
				if(handle_flag!=1)    //û��������ȴ�һ��
				{ 
						wait_time++;
						delay_ms(1);        //�ӳ�1ms�ȴ����ȴ�3��
						continue;
				}
				
				//dtu�˵Ĵ���
				Modbus_Handle();
				
//			 printf("���ݴ������\r\n");
			  handle_flag=0;         //�����־λ��0
				
				
		   
	     break;
		}
		
		delay_ms(1000);     //�ӳ�һ��
				   
	 } 
}


