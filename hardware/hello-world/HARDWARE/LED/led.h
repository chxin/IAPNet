#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define LED1 PCout(6)//高电平点亮，低电平关断
#define LED2 PCout(7)//
#define LED3 PCout(8)// 	

#define PWMOUT1 PBout(6)		//光电管驱动
#define PWMOUT2 PBout(5)		//阻抗驱动
//#define PWMOUT3 PBout(7)
//#define PWMOUT4 PBout(8)

void LED_PWMIO_Init(void);

		 				    
#endif
