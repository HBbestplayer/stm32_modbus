#include "timing.h"
#include "sys.h"
#include "rs485.h"
#include <stdio.h>
#include "usart.h"

/******************************************************************************
  * @brief  选择外部时钟或者内部时钟并进行倍频
  * @param
	RCC_PLLSource：PLL时钟源 :
						可以选择：RCC_PLLSource_HSI_Div2、RCC_PLLSource_HSE_Div2、RCC_PLLSource_HSE_Div1
	PLLMUL：PLL输入时钟的倍频系数
			  范围：RCC_CFGR_PLLMULL2~16
						PLL时钟根据时钟和倍频来确定，选择内部时钟最高64M
  * @retval
 ******************************************************************************/
void SysClock_Configuration(u32 RCC_PLLSource, u32 PLLMUL)
{
	__IO u32 HSEStatus = 0;                      //外部时钟标志位

	RCC_ClocksTypeDef get_rcc_clock;

	RCC_DeInit();                                // 初始化

	if (RCC_PLLSource_HSI_Div2 != RCC_PLLSource) //选择外部时钟或内部时钟
	{
		RCC_HSEConfig(RCC_HSE_ON);				         //打开外部时钟
		if (RCC_WaitForHSEStartUp() == SUCCESS)    //等待外部时钟开启
		{
			HSEStatus = 1;
		}
		else
		{											                     //外部时钟打开失败
			RCC_PLLSource = RCC_PLLSource_HSI_Div2;  //自动选择内部时钟
			PLLMUL = RCC_CFGR_PLLMULL16;			       //倍频到64MHZ
			RCC_HSEConfig(RCC_HSE_OFF);				       //关闭外部时钟
			RCC_HSICmd(ENABLE);						           //打开内部时钟
		}
	}
	else
	{											                      //内部时钟
		RCC_PLLSource = RCC_PLLSource_HSI_Div2;   //自动选择内部时钟
		PLLMUL = RCC_CFGR_PLLMULL16;			        //倍频到64MHZ
		RCC_HSEConfig(RCC_HSE_OFF);				        //关闭外部时钟
		RCC_HSICmd(ENABLE);						            //打开内部时钟
	}

	RCC_HCLKConfig(RCC_SYSCLK_Div1);           // HCLK（AHB）时钟为系统时钟1分频
	RCC_PCLK1Config(RCC_HCLK_Div2);	           // PCLK（APB1）时钟为HCLK时钟2分频
	RCC_PCLK2Config(RCC_HCLK_Div1);	           // PCLK（APB2）时钟为HCLK时钟1分频

	// 0-24MHz时,取FLASH_Latency_0;
	// 24-48MHz,取FLASH_Latency_1;
	// 48-72MHz时,取FLASH_Latency_2。
	//防止跑飞
	FLASH_SetLatency(FLASH_Latency_2); 
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	RCC_PLLConfig(RCC_PLLSource, PLLMUL);      // PLL时钟配置，时钟源 * PLLMUL

	RCC_PLLCmd(ENABLE);                        //开启PLL时钟，并等待PLL时钟准备好
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //选择PLL时钟为系统时钟

	while (RCC_GetSYSCLKSource() != 0x08);		 // 等到系统时钟源为PLL
	RCC_ClockSecuritySystemCmd(ENABLE);        //打开时钟安全系统
	
	RCC_GetClocksFreq(&get_rcc_clock);         //仿真的时候就可以在结构体get_rcc_clock中看见各个外设的时钟了

}

/******************************************************************************
  * @brief  MODBUS_TIM_Config：使用TIM3作为定时器，TIM3初始化
  * @param  
  * @retval 
 ******************************************************************************/
void MODBUS_TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      //开启定时器时钟,即内部时钟CK_INT=72M
	
  TIM_TimeBaseStructure.TIM_Period=MODBUS_TIM_Period;        //自动重装载寄存器周的值(计数值)
  // 累计TIM_Period 个频率后产生一个更新或者中断
  // 时钟预分频数为71，则驱动计数器的时钟CK_CNT = CK_INT / (71+1)=1M
  TIM_TimeBaseStructure.TIM_Prescaler= MODBUS_TIM_Prescaler;
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     // 时钟分频因子 ，基本定时器没有，不用管
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // 计数器计数模式，基本定时器只能向上计数，没有计数模式的设置
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;            // 重复计数器的值，基本定时器没有，不用管
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);      // 初始化定时器
  TIM_ClearFlag(TIM3,TIM_FLAG_Update);                // 清除计数器中断标志位
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);            // 开启计数器中断
  TIM_Cmd(TIM3, ENABLE);                              // 使能计数器
 }
/******************************************************************************
  * @brief  ALL_TIM_NVIC_Init：配置时间中断优先级
  * @param  
  * @retval 
 ******************************************************************************/


void ALL_TIM3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // 设置中断组为2	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;            // 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;   // 设置抢占优先级为3，优先级比接收中断低
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          // 设置响应优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = 2000-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =MODBUS_TIM_Prescaler ; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);                // 清除计数器中断标志位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // 设置中断组为2
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	
}

void TIM3_Init(void)
	
{
	 MODBUS_TIM_Config();
	 ALL_TIM3_NVIC_Init();
	
}


extern rtu_modbus modbus;

void TIM4_IRQHandler(void)   //TIM4中断
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
		{
		   
			   if(modbus.timerun != 0)
				 {	 
		        modbus.timeout++;           //定时器定时1毫秒，并开始记时

						if (modbus.timeout > 2)     // 3.5个字符时间，9600波特率的话，经过计算在4ms左右
						{   
//								printf("rtu帧结束\r\n");
								modbus.timerun = 0;          //关闭定时器
								modbus.timeout = 0;          //定时器计数置0
								modbus.reflag = 1;
							
							 //如果此时帧结束，数据有问题要将数据清零，重新接收
				
						}
					}
				 
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
		}
}



/******************************************************************************
 * @brief  MODBUS_TIM_IRQHandler：MODBUS定时器中断函数
 * @param
 * @retval
 ******************************************************************************/



	
extern tcp_modbus tcpbus;


void TIM3_IRQHandler(void) //定时器中断函数
{           

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //TIM_IT_Update,更新中断，计数器向上溢出/向下溢出，计数器初始化(通过软件或者内部/外部触发) 
    {   
				 if (tcpbus.timerun != 0)   //只有tcp需要定时
					{
							tcpbus.timeout++;           //定时器定时1毫秒，并开始记时

							if (tcpbus.timeout > 8)  // 3.5个字符时间，9600波特率的话，经过计算4ms左右
							{   
//									printf("tcp帧结束\r\n");
									tcpbus.timerun = 0;          //关闭定时器
									tcpbus.timeout = 0;          //计数置0
									tcpbus.recflag = 1;


							}
					}
			 
			 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位,原来是TIM_FLAG_Update
    }
}

