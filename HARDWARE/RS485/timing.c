#include "timing.h"
#include "sys.h"
#include "rs485.h"
#include <stdio.h>
#include "usart.h"

/******************************************************************************
  * @brief  ѡ���ⲿʱ�ӻ����ڲ�ʱ�Ӳ����б�Ƶ
  * @param
	RCC_PLLSource��PLLʱ��Դ :
						����ѡ��RCC_PLLSource_HSI_Div2��RCC_PLLSource_HSE_Div2��RCC_PLLSource_HSE_Div1
	PLLMUL��PLL����ʱ�ӵı�Ƶϵ��
			  ��Χ��RCC_CFGR_PLLMULL2~16
						PLLʱ�Ӹ���ʱ�Ӻͱ�Ƶ��ȷ����ѡ���ڲ�ʱ�����64M
  * @retval
 ******************************************************************************/
void SysClock_Configuration(u32 RCC_PLLSource, u32 PLLMUL)
{
	__IO u32 HSEStatus = 0;                      //�ⲿʱ�ӱ�־λ

	RCC_ClocksTypeDef get_rcc_clock;

	RCC_DeInit();                                // ��ʼ��

	if (RCC_PLLSource_HSI_Div2 != RCC_PLLSource) //ѡ���ⲿʱ�ӻ��ڲ�ʱ��
	{
		RCC_HSEConfig(RCC_HSE_ON);				         //���ⲿʱ��
		if (RCC_WaitForHSEStartUp() == SUCCESS)    //�ȴ��ⲿʱ�ӿ���
		{
			HSEStatus = 1;
		}
		else
		{											                     //�ⲿʱ�Ӵ�ʧ��
			RCC_PLLSource = RCC_PLLSource_HSI_Div2;  //�Զ�ѡ���ڲ�ʱ��
			PLLMUL = RCC_CFGR_PLLMULL16;			       //��Ƶ��64MHZ
			RCC_HSEConfig(RCC_HSE_OFF);				       //�ر��ⲿʱ��
			RCC_HSICmd(ENABLE);						           //���ڲ�ʱ��
		}
	}
	else
	{											                      //�ڲ�ʱ��
		RCC_PLLSource = RCC_PLLSource_HSI_Div2;   //�Զ�ѡ���ڲ�ʱ��
		PLLMUL = RCC_CFGR_PLLMULL16;			        //��Ƶ��64MHZ
		RCC_HSEConfig(RCC_HSE_OFF);				        //�ر��ⲿʱ��
		RCC_HSICmd(ENABLE);						            //���ڲ�ʱ��
	}

	RCC_HCLKConfig(RCC_SYSCLK_Div1);           // HCLK��AHB��ʱ��Ϊϵͳʱ��1��Ƶ
	RCC_PCLK1Config(RCC_HCLK_Div2);	           // PCLK��APB1��ʱ��ΪHCLKʱ��2��Ƶ
	RCC_PCLK2Config(RCC_HCLK_Div1);	           // PCLK��APB2��ʱ��ΪHCLKʱ��1��Ƶ

	// 0-24MHzʱ,ȡFLASH_Latency_0;
	// 24-48MHz,ȡFLASH_Latency_1;
	// 48-72MHzʱ,ȡFLASH_Latency_2��
	//��ֹ�ܷ�
	FLASH_SetLatency(FLASH_Latency_2); 
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	RCC_PLLConfig(RCC_PLLSource, PLLMUL);      // PLLʱ�����ã�ʱ��Դ * PLLMUL

	RCC_PLLCmd(ENABLE);                        //����PLLʱ�ӣ����ȴ�PLLʱ��׼����
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //ѡ��PLLʱ��Ϊϵͳʱ��

	while (RCC_GetSYSCLKSource() != 0x08);		 // �ȵ�ϵͳʱ��ԴΪPLL
	RCC_ClockSecuritySystemCmd(ENABLE);        //��ʱ�Ӱ�ȫϵͳ
	
	RCC_GetClocksFreq(&get_rcc_clock);         //�����ʱ��Ϳ����ڽṹ��get_rcc_clock�п������������ʱ����

}

/******************************************************************************
  * @brief  MODBUS_TIM_Config��ʹ��TIM3��Ϊ��ʱ����TIM3��ʼ��
  * @param  
  * @retval 
 ******************************************************************************/
void MODBUS_TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      //������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	
  TIM_TimeBaseStructure.TIM_Period=MODBUS_TIM_Period;        //�Զ���װ�ؼĴ����ܵ�ֵ(����ֵ)
  // �ۼ�TIM_Period ��Ƶ�ʺ����һ�����»����ж�
  // ʱ��Ԥ��Ƶ��Ϊ71����������������ʱ��CK_CNT = CK_INT / (71+1)=1M
  TIM_TimeBaseStructure.TIM_Prescaler= MODBUS_TIM_Prescaler;
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     // ʱ�ӷ�Ƶ���� ��������ʱ��û�У����ù�
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; // ����������ģʽ��������ʱ��ֻ�����ϼ�����û�м���ģʽ������
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;            // �ظ���������ֵ��������ʱ��û�У����ù�
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);      // ��ʼ����ʱ��
  TIM_ClearFlag(TIM3,TIM_FLAG_Update);                // ����������жϱ�־λ
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);            // �����������ж�
  TIM_Cmd(TIM3, ENABLE);                              // ʹ�ܼ�����
 }
/******************************************************************************
  * @brief  ALL_TIM_NVIC_Init������ʱ���ж����ȼ�
  * @param  
  * @retval 
 ******************************************************************************/


void ALL_TIM3_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // �����ж���Ϊ2	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn ;            // �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;   // ������ռ���ȼ�Ϊ3�����ȼ��Ƚ����жϵ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;          // ������Ӧ���ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 2000-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =MODBUS_TIM_Prescaler ; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_ClearFlag(TIM4,TIM_FLAG_Update);                // ����������жϱ�־λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);             // �����ж���Ϊ2
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	
}

void TIM3_Init(void)
	
{
	 MODBUS_TIM_Config();
	 ALL_TIM3_NVIC_Init();
	
}


extern rtu_modbus modbus;

void TIM4_IRQHandler(void)   //TIM4�ж�
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
		{
		   
			   if(modbus.timerun != 0)
				 {	 
		        modbus.timeout++;           //��ʱ����ʱ1���룬����ʼ��ʱ

						if (modbus.timeout > 2)     // 3.5���ַ�ʱ�䣬9600�����ʵĻ�������������4ms����
						{   
//								printf("rtu֡����\r\n");
								modbus.timerun = 0;          //�رն�ʱ��
								modbus.timeout = 0;          //��ʱ��������0
								modbus.reflag = 1;
							
							 //�����ʱ֡����������������Ҫ���������㣬���½���
				
						}
					}
				 
				TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		}
}



/******************************************************************************
 * @brief  MODBUS_TIM_IRQHandler��MODBUS��ʱ���жϺ���
 * @param
 * @retval
 ******************************************************************************/



	
extern tcp_modbus tcpbus;


void TIM3_IRQHandler(void) //��ʱ���жϺ���
{           

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //TIM_IT_Update,�����жϣ��������������/�����������������ʼ��(ͨ����������ڲ�/�ⲿ����) 
    {   
				 if (tcpbus.timerun != 0)   //ֻ��tcp��Ҫ��ʱ
					{
							tcpbus.timeout++;           //��ʱ����ʱ1���룬����ʼ��ʱ

							if (tcpbus.timeout > 8)  // 3.5���ַ�ʱ�䣬9600�����ʵĻ�����������4ms����
							{   
//									printf("tcp֡����\r\n");
									tcpbus.timerun = 0;          //�رն�ʱ��
									tcpbus.timeout = 0;          //������0
									tcpbus.recflag = 1;


							}
					}
			 
			 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ,ԭ����TIM_FLAG_Update
    }
}

