#ifndef _TIMING_H
#define _TIMING_H

#include "rs485.h"
#include "usart.h"



// 使用TIM3，对MODBUS协议定时
#define MODBUS_TIM                   TIM3             
#define MODBUS_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd  //使能APB1
#define MODBUS_TIM_CLK               RCC_APB1Periph_TIM3     //外设时钟选择TIM3
#define MODBUS_TIM_IRQ               TIM3_IRQn
#define MODBUS_TIM_IRQHandler        TIM3_IRQHandler
#define MODBUS_TIM_Period            (500-1)  //重载值，500次,即0.5ms
#define MODBUS_TIM_Prescaler         (72-1)//psc分频

//情况定义
 

//extern unsigned char  time_case;



void SysClock_Configuration(unsigned int RCC_PLLSource, unsigned int PLLMUL);
void MODBUS_TIM_Config(void);
void ALL_TIM3_NVIC_Init(void);
void TIM4_Init(void);
void TIM3_Init(void);




#endif

