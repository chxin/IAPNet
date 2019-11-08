#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  


typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   


void iap_load_app(u32 appxaddr);			//跳转程序执行
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//在指定地址开始,写入bin

#endif







































