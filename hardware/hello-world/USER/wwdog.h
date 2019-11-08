#ifndef __WWDOG_H
#define __WWDOG_H	 


#include "stm32f10x.h"
#include "sys.h"
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);

void WWDG_Init(u8 tr,u8 wr,u32 fprer);//��ʼ��WWDG
void WWDG_Set_Counter(u8 cnt);       	//����WWDG�ļ�����
void WWDG_NVIC_Init(void);

void TIM4_Int_Init(u16 arr,u16 psc);

 
#endif
