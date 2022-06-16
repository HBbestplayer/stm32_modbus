//5月29号修改
//修改定时器，将一个定时器改成两个定时器，TIM3对dtu端的指令定时，TIM4对传感器端的指令定时
//修改中断间的优先级，dtu端的优化级最高，次而是传感器端，最后是两个定时器。
//修改了接收中断，将接收中断里的printf删掉，将波特率提高
//将tcp的帧格式作了更好地规定，地址码+功能码+起始地址×2+个数×2+数据×2+校验码×2

//6月3号修改
//修改了printf，重定向到串口3打印输出
//串口1添加两个收发控制位，控制对应io口高低电平



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
	
	extern u32 threshold;  //设置的阈值
	
	//定时器
	SysClock_Configuration(RCC_PLLSource_HSE_Div1, RCC_CFGR_PLLMULL9);
	TIM3_Init();
	TIM4_Init();
	
	//延时函数初始化
	delay_init();	    	 	  
	
	//串口2-485初始化
	
	RS485_Init(9600);	//初始化RS485
	
	//串口1初始化
	uart_init(9600);	 	//串口初始化为9600
	
	
	LED_Init();		  		//初始化与LED连接的硬件接口	 	 

	
	while(1)
	{ 
    modbus.reflag=0;		  //发送前，先将接收标志位置0
//		tcpbus.recflag=0;
		RS485_Send_Data(getdata_buf,8);
		delay_ms(500);

		while(1)
		{
			//闪烁LED,提示系统正在运行
		   LED0=!LED0;

			 if(wait_time>2)
				 {
						 wait_time=0;
						 modbus.recount=0;
						 break;                    //等待15ms,不行就重新发送查询
			   }
				 		 

				 
				 
				//传感器端的处理
				Check_Modbus(modbus.recbuf, modbus.recount);  
				if(handle_flag!=1)    //没处理过，等待一下
				{ 
						wait_time++;
						delay_ms(1);        //延迟1ms等待，等待3次
						continue;
				}
				
				//dtu端的处理
				Modbus_Handle();
				
//			 printf("数据处理完成\r\n");
			  handle_flag=0;         //处理标志位置0
				
				
		   
	     break;
		}
		
		delay_ms(1000);     //延迟一秒
				   
	 } 
}


