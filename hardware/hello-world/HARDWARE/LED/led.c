

#include "led.h"

 
//LED IO初始化
void LED_PWMIO_Init(void)
{
 
 	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOBC时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速率50M  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;		
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化		   
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速率50M  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5;		//PB5为阻抗PWM输出口，PB6为光电调制输出口
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化	

		/* 置位所有led灯	*/ 
  GPIO_ResetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_5);
	
}
