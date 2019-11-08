#ifndef __RS485_H
#define __RS485_H			



#include "stm32f10x.h"



//#define RS485_TX_EN		PEout(4)		//485模式控制.0,接收;1,发送.
#define LENGTH  30*1024    					//接收数据缓冲长度30K
#define CMD_LEN 7
#define TestAddr 0x20								//测试用地址

extern u8 Mode_Flag;    						//接收标志位 
extern u32 RxCnt; 
extern uint8_t Cmd_Pack[CMD_LEN];
extern u8  RxBuf[LENGTH];
extern u32 Rcv_Flag;
extern uint8_t Send_Pack[CMD_LEN + 1];

#define RS485_TX_PORT				GPIOA
#define RS485_TX_PIN				GPIO_Pin_2
#define RS485_CLK                   RCC_APB2Periph_GPIOA

#define RS485_RX_PORT				GPIOA
#define RS485_RX_PIN				GPIO_Pin_3

#define RS485_DIR_PORT              GPIOB
#define RS485_DIR_PIN				GPIO_Pin_9
#define RS485_DIR_CLK				RCC_APB2Periph_GPIOB

#define RS485_DIR_EN  PBout(9)    //1为发送，0为接收


#define START_BYTE1     0x24
#define START_BYTE2     0x5F
#define START_BYTE3     0x24

#define IAP_Error		0x0e  	//IAP错误
#define APP1_Error		0x1e  	//APP1加载错误
#define APP2_Error		0x2e  	//APP2加载错误
#define IAP_ACK         0x0a  	//IAP指令响应
#define APP1_ACK		0x1a  	//APP1指令响应
#define APP2_ACK		0x2a  	//APP2指令响应
#define IAP_NACK        0x05  	//IAP指令未响应
#define APP1_NACK		0x15  	//APP1指令未响应
#define APP2_NACK		0x25  	//APP2指令未响应
#define Unknow_Error    0x55  	//未知错误

#define IAP_Running		0x0c  	//正在运行IAP
#define APP1_Running    0x1c  	//正在运行APP1
#define APP2_Running    0x2c  	//正在运行APP2

#define FLASH_IAP_ADDR		0x08000000  	//IAP程序起始地址(存放在FLASH)
//保留0x08000000~0x0800FFFF的空间为Bootloader使用（64KB）
#define FLASH_APP1_ADDR		0x08010000  	//APP程序1起始地址(存放在FLASH)
//保留0X08010000~0X0802FFFF的空间为Bootloader使用（128KB）												
#define FLASH_APP2_ADDR		0x08030000  	//APP程序2起始地址(存放在FLASH)

#define FLASH_Running_Flag_ADDR       0x08070000
#define Current_Version_ADDR       FLASH_Running_Flag_ADDR + 4

typedef enum
{
    Run_IAP,
    Run_APP1,
    Run_APP2,
    Load_APP,
    Acquire_software_Version,
    Acquire_Data
}CMD;


typedef enum
{
	Addr,
    FirstStart,
    SecondStart,
    ThirdStart,
    Cmd,
    Chksum1,
    Chksum2
} RxState;


u16 crc16(const u8 *puchMsg, u16 usDataLen);
void RS485_Init(u32 bound);
void RS485_SendData(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);
void RS232_Send_Data(u8 *buf,u16 len);
void RS485_Sendbyte(u8 c);
void Data_UnPack_Process(u8 c);
void CMD_Process(u8 cmd_value);
	
#endif	   
















