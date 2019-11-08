

#include "led.h"

 
//LED IO��ʼ��
void LED_PWMIO_Init(void)
{
 
 	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBCʱ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����50M  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;		
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��		   
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����50M  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5;		//PB5Ϊ�迹PWM����ڣ�PB6Ϊ�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��	

		/* ��λ����led��	*/ 
  GPIO_ResetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
	GPIO_ResetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_5);
	
}
