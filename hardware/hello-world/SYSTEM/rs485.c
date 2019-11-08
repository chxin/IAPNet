#include "rs485.h"
#include "delay.h"
#include "sys.h"
#include "main.h"
#include "led.h"
#include "iap.h"
#include "usart.h"	
#include "stmflash.h"

u8  RxBuf[LENGTH]  __attribute__ ((at(0X20000a00)));//���ջ���,���USART_REC_LEN���ֽ�,��ʼ��ַΪ0X20001000.
u8  Mode_Flag = 0xff;  //ģʽ�л���־λ
u32 RxCnt = 0;

/*****
*@file  		rs485.c
*@author 		xiaoluo
*@date   		2018/01/09
*@function: RS485_Init(u32 bound)
						RS485_Send_Data(u8 *buf,u8 len)
						USART2_DMA_Init(u32 cnt)
*@brief     ʹ��DMA���䴮�����ݣ����Ը��ļ���Ҫ��ʼ��USART2��DMA_USART2_TX
						���������
*@attention 1.USART2_DMA������Ҫ��ȷ
						2.Ϊ�˴������ݵĹ����У������ݲ��ḱ�����ڴ�������ݣ�
						��ÿ�����ݴ����꣬�����ж�ʱ��Ҫ��DMAʹ�ܣ��ȴ���һ������
						3.�����������Ѿ��궨����ˣ����к�ǿ����Ӧ��
******/



/* CRC ��λ�ֽ�ֵ�� */
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

// CRC��λ�ֽ�ֵ��
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


u16 crc16(const u8 *puchMsg, u16 usDataLen)  //const������ʽ���������Ա������飬���������ڴ�������Ĺ����У����鵱�����ƶ�����
{
    u8 uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */
    u8 uchCRCLo = 0xFF ; /* ��CRC �ֽڳ�ʼ�� */
    u32 uIndex ; /* CRCѭ���е����� */

    while (usDataLen--)   /* ������Ϣ������ */
    {
        uIndex = uchCRCHi ^ *puchMsg++ ; /* ����CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }

    return(uchCRCHi << 8 | uchCRCLo);
}


/*
*****************************************************************
*	�� �� ��: RS485_GPIO_Config
*	����˵��: RS485��������ģʽ��ʼ��
*	��    �Σ���
*	�� �� ֵ: ��
******************************************************************/
void RS485_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RS485_CLK | RS485_DIR_CLK, ENABLE);	   //ʹ�ܶ˿�ʱ��

    //485_DIR  PB9
    GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN;				                     //�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		                   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		                   //IO���ٶ�Ϊ50MHz
    GPIO_Init(RS485_DIR_PORT, &GPIO_InitStructure);			                   //�����趨������ʼ��GPIO

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
*	�� �� ��: RS485_Mode_Config
*	����˵��: 485ģʽ����
*	��    ��: bound ������
*	�� �� ֵ: ��
**************************************************************/


void RS485_Mode_Config(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    //USART2 NVIC ����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;                //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		                 //�����ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			                   //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	                                       //����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART2 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;                           //���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;           //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                   //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	                  //�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure);                            //��ʼ������1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                       //�������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                                           //ʹ�ܴ���1

    RS485_DIR_EN = 0;			//Ĭ������Ϊ����״̬
}


/*
*************************************************************
*	�� �� ��: RS485_Init
*	����˵��: RS485��ʼ��
*	��    ��: bound ������
*	�� �� ֵ: ��
**************************************************************/
void RS485_Init(u32 bound)
{
    RS485_GPIO_Config();
    RS485_Mode_Config(bound);
}



void RS485_SendData(u8 *pBuffer, u8 length)
{
    RS485_DIR_EN = 1;			//����Ϊ����ģʽ
    delay_ms(5);
    USART_GetFlagStatus(USART2, USART_FLAG_TC);
    while(length--)
    {
        USART_SendData(USART2,*pBuffer);
        pBuffer++;
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
    delay_ms(5);
    RS485_DIR_EN = 0;				//����Ϊ����ģʽ	                                                //������ɺ��ٴ�����Ϊ����״̬
}

void RS485_Sendbyte(u8 c)
{
    RS485_DIR_EN = 1;			//����Ϊ����ģʽ
    USART_SendData(USART2,c);    
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    RS485_DIR_EN = 0;				//����Ϊ����ģʽ	                                                //������ɺ��ٴ�����Ϊ����״̬
}

u32 Rcv_Flag = 2;
/*
*************************************************************
*	�� �� ��: USART2_IRQHandler
*	����˵��: ���ж���������ݽ���
*	��    ��: ��
*	�� �� ֵ: ��
**************************************************************/
void USART2_IRQHandler(void)                	                		//����2�жϷ������
{
    u8 res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        res = USART2->DR;
                 		       
        if(Mode_Flag == 3)//�����ļ�
        {
            
            if(RxCnt < LENGTH)  //��ֹ����Խ��
            {
                RxBuf[RxCnt] = USART_ReceiveData(USART2);
                RxCnt++;
                Rcv_Flag = RxCnt;			 									     
            }
            
        }
        else//��������
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
                //RS485_Sendbyte(0xFF); //����֡���� ����0xFF
                //printf("Data");
					}
			break;
            
			case Chksum2:
					if(cksum[1] == c)   //����֡��ȷ ���¿�ʼ����
					{
						Mode_Flag = cmd_mode;

						CMD_Process(cmd_mode);//�������֡����

					}
					else
					{    
                //RS485_Sendbyte(0xFF); //����֡���� ����0xFF
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
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0xFFFF);//д��flag 
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0002);//д�뵱ǰ����汾 
		break;

		case Run_APP1:
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0x1234);//д��flag
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0330);//д�뵱ǰ����汾 
		break;

		case Run_APP2:
            FLASH_WriteHalfWord(FLASH_Running_Flag_ADDR,0x4567);//д��flag
            FLASH_WriteHalfWord(Current_Version_ADDR,0x0330);//д�뵱ǰ����汾 
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



