#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  


typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   


void iap_load_app(u32 appxaddr);			//��ת����ִ��
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin

#endif







































