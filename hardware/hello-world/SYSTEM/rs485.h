#ifndef __RS485_H
#define __RS485_H			



#include "stm32f10x.h"



//#define RS485_TX_EN		PEout(4)		//485ģʽ����.0,����;1,����.
#define LENGTH  30*1024    					//�������ݻ��峤��30K
#define CMD_LEN 7
#define TestAddr 0x20								//�����õ�ַ

extern u8 Mode_Flag;    						//���ձ�־λ 
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

#define RS485_DIR_EN  PBout(9)    //1Ϊ���ͣ�0Ϊ����


#define START_BYTE1     0x24
#define START_BYTE2     0x5F
#define START_BYTE3     0x24

#define IAP_Error		0x0e  	//IAP����
#define APP1_Error		0x1e  	//APP1���ش���
#define APP2_Error		0x2e  	//APP2���ش���
#define IAP_ACK         0x0a  	//IAPָ����Ӧ
#define APP1_ACK		0x1a  	//APP1ָ����Ӧ
#define APP2_ACK		0x2a  	//APP2ָ����Ӧ
#define IAP_NACK        0x05  	//IAPָ��δ��Ӧ
#define APP1_NACK		0x15  	//APP1ָ��δ��Ӧ
#define APP2_NACK		0x25  	//APP2ָ��δ��Ӧ
#define Unknow_Error    0x55  	//δ֪����

#define IAP_Running		0x0c  	//��������IAP
#define APP1_Running    0x1c  	//��������APP1
#define APP2_Running    0x2c  	//��������APP2

#define FLASH_IAP_ADDR		0x08000000  	//IAP������ʼ��ַ(�����FLASH)
//����0x08000000~0x0800FFFF�Ŀռ�ΪBootloaderʹ�ã�64KB��
#define FLASH_APP1_ADDR		0x08010000  	//APP����1��ʼ��ַ(�����FLASH)
//����0X08010000~0X0802FFFF�Ŀռ�ΪBootloaderʹ�ã�128KB��												
#define FLASH_APP2_ADDR		0x08030000  	//APP����2��ʼ��ַ(�����FLASH)

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
















