#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define LED1 PCout(6)//�ߵ�ƽ�������͵�ƽ�ض�
#define LED2 PCout(7)//
#define LED3 PCout(8)// 	

#define PWMOUT1 PBout(6)		//��������
#define PWMOUT2 PBout(5)		//�迹����
//#define PWMOUT3 PBout(7)
//#define PWMOUT4 PBout(8)

void LED_PWMIO_Init(void);

		 				    
#endif
