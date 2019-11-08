#include "rs485.h"
#include "delay.h"
#include "sys.h"
#include "main.h"
#include "led.h"
#include "iap.h"
#include "usart.h"	
#include "stmflash.h"

u8  RxBuf[LENGTH]  __attribute__ ((at(0X20000a00)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.
u8  Mode_Flag = 0xff;  //模式切换标志位
u32 RxCnt = 0;

/*****
*@file  		rs485.c
*@author 		xiaoluo
*@date   		2018/01/09
*@function: RS485_Init(u32 bound)
						RS485_Send_Data(u8 *buf,u8 len)
						USART2_DMA_Init(u32 cnt)
*@brief     使用DMA传输串口数据，所以该文件主要初始化USART2和DMA_USART2_TX
						的相关配置
*@attention 1.USART2_DMA的配置要正确
						2.为了传输数据的过程中，新数据不会副高正在传输的数据，
						当每次数据传输完，触发中断时，要将DMA使能，等待新一次数据
						3.传输数据量已经宏定义好了，具有很强的适应性
******/



/* CRC 高位字节值表 */
const u8 auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

// CRC低位字节值表
const u8 auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;


u16 crc16(const u8 *puchMsg, u16 usDataLen)  //const声明形式变量，可以保护数组，这样函数在处理数组的过程中，数组当做是笃定不变
{
    u8 uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
    u8 uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
    u32 uIndex ; /* CRC循环中的索引 */

    while (usDataLen--)   /* 传输消息缓冲区 */
    {
        uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }

    return(uchCRCHi << 8 | uchCRCLo);
}


/*
*****************************************************************
*	函 数 名: RS485_GPIO_Config
*	功能说明: RS485方向引脚模式初始化
*	形    参：无
*	返 回 值: 无
******************************************************************/
void RS485_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RS485_CLK | RS485_DIR_CLK, ENABLE);	   //使能端口时钟

    //485_DIR  PB9
    GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;				                     //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                   //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                   //IO口速度为50MHz
    GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);			                   //根据设定参数初始化GPIO

    //USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = RS485_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = RS485_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
*************************************************************
*	函 数 名: RS485_Mode_Config
*	功能说明: 485模式设置
*	形    参: bound 波特率
*	返 回 值: 无
**************************************************************/


void RS485_Mode_Config(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    //USART2 NVIC 配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;                //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                 //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			                   //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	                                       //根据指定的参数初始化VIC寄存器

    //USART2 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                           //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;           //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                   //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                  //收发模式

    USART_Init(USART2, &USART_InitStructure);                            //初始化串口1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                       //开启串口接受中断
    USART_Cmd(USART2, ENABLE);                                           //使能串口1

    RS485_DIR_EN = 0;			//默认设置为接收状态
}


/*
*************************************************************
*	函 数 名: RS485_Init
*	功能说明: RS485初始化
*	形    参: bound 波特率
*	返 回 值: 无
**************************************************************/
void RS485_Init(u32 bound)
{
    RS485_GPIO_Config();
    RS485_Mode_Config(bound);
}



void RS485_SendData(u8 *pBuffer, u8 length)
{
    RS485_DIR_EN = 1;			//设置为发送模式
    delay_ms(5);
    USART_GetFlagStatus(USART2, USART_FLAG_TC);
    while(length--)
    {
        USART_SendData(USART2,*pBuffer);
        pBuffer++;
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
    delay_ms(5);
    RS485_DIR_EN = 0;				//设置为接收模式	                                                //调用完成后再次设置为接收状态
}

void RS485_Sendbyte(u8 c)
{
    RS485_DIR_EN = 1;			//设置为发送模式
    USART_SendData(USART2,c);    
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    RS485_DIR_EN = 0;				//设置为接收模式	                                                //调用完成后再次设置为接收状态
}

u32 Rcv_Flag = 2;
/*
*************************************************************
*	函 数 名: USART2_IRQHandler
*	功能说明: 在中断中完成数据接收
*	形    参: 无
*	返 回 值: 无
**************************************************************/
void USART2_IRQHandler(void)                	                		//串口2中断服务程序
{
    u8 res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        res = USART2->DR;
                 		       
        if(Mode_Flag == 3)//接收文件
        {
            
            if(RxCnt < LENGTH)  //防止数组越界
            {
                RxBuf[RxCnt] = USART_ReceiveData(USART2);
                RxCnt++;
                Rcv_Flag = RxCnt;			 									     
            }
            
        }
        else//接收命令
        {
            Data_UnPack_Process(res);
        }
//        if(Mode_Flag == 0)


    }
}

static volatile RxState state = Addr;
static volatile uint8_t cmd_mode = 0;
static volatile uint8_t cksum[2] = {0};
uint8_t Cmd_Pack[CMD_LEN] = {0};
uint8_t Cmd_RxCnt = 0;

uint8_t Send_Pack[CMD_LEN + 1] = {TestAddr,START_BYTE1,START_BYTE2,START_BYTE3};

u32 Program_Addr = 0;
u32 BackUp_Addr = 0;


void Data_UnPack_Process(u8 c)
{
	Cmd_Pack[Cmd_RxCnt++] = c;
	
	if(Cmd_RxCnt <= CMD_LEN)
	{
    switch(state)
		{
			case Addr:
			state = (c == TestAddr) ? FirstStart : Addr;
//			if(Addr == state) 
//				{					
//					Cmd_RxCnt = 0;
//				}
			break;

			case FirstStart:
				state = (c == START_BYTE1) ? SecondStart : FirstStart;
//				if(FirstStart == state)
//				{	
//					Cmd_RxCnt = 0;
//					state = Addr;
//				}
				break;

			case SecondStart:
					state = (c == START_BYTE2) ? ThirdStart : SecondStart;
//					if(SecondStart == state)
//					{					
//						Cmd_RxCnt = 0;
//						state = Addr;
//					}
			break;

			case ThirdStart:
					state = (c == START_BYTE3) ? Cmd : ThirdStart;
//					if(ThirdStart == state)
//					{					
//							Cmd_RxCnt = 0;
//							state = Addr;
//					}
			break;

			case Cmd:
					cmd_mode = c;
					state = Chksum1;
			break;

			case Chksum1:
					cksum[0] = crc16(Cmd_Pack, 5)>>8;
					cksum[1] = (crc16(Cmd_Pack, 5)&0xFF);
					if(cksum[0] == c)
					{
						state = Chksum2;
					}
					else
					{
						state = Addr;
						Cmd_RxCnt = 0;
                //RS485_Sendbyte(0xFF); //接收帧错误 返回0xFF
                //printf("Data");
					}
			break;
            
			case Chksum2:
					if(cksum[1] == c)   //接收帧正确 重新开始接收
					{
						Mode_Flag = cmd_mode;

						CMD_Process(cmd_mode);//处理接收帧命令

					}
					else
					{    
                //RS485_Sendbyte(0xFF); //接收帧错误 返回0xFF
					}
						Cmd_RxCnt = 0;
            state = Addr;
				break;
        default:

        break;
		
		
			}
	
	}
	else
	{
		Cmd_RxCnt = 0;
	}

}

void CMD_Process(u8 cmd_value)
{
    
	switch(cmd_value)
	{

		case Run_IAP:
            //Program_Addr = 0;
            //BackUp_Addr = 0;    
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0xFFFF);//写入flag 
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0002);//写入当前软件版本 
		break;

		case Run_APP1:
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0x1234);//写入flag
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0330);//写入当前软件版本 
		break;

		case Run_APP2:
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0x4567);//写入flag
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0330);//写入当前软件版本 
		break;

        case Load_APP:
            
		break;

        case Acquire_software_Version:
            Send_Pack[4] = STMFLASH_ReadHalfWord(Current_Version_ADDR)>>8;
            Send_Pack[5] = STMFLASH_ReadHalfWord(Current_Version_ADDR)&0xFF;
            Send_Pack[6] = crc16(Send_Pack, 6)>>8;
            Send_Pack[7] = (crc16(Send_Pack, 6)&0xFF);
            RS485_SendData(Send_Pack, sizeof(Send_Pack));
		break;

		case Acquire_Data:
                       
        break;
        default:

        break;
    }
    
}



