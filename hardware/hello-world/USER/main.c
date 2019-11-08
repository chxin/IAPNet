#include "main.h"
#include "usart.h"
#include "delay.h"
#include "rs485.h"
#include "led.h"
#include "iap.h"
#include "stmflash.h"

static volatile u16  FLASH_Working_Mode;//appģʽ�л���־λ
static volatile u16 APP_Version;
uint8_t bin_crc[2] = {0};



/*
0x1234--AAP1
0x4567--AAP2
other--IAP
*/

int main(void)
{
    
    //uint8_t i,j;
    delay_init();	    					//��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//3λ��ռ���ȼ���1λ��Ӧ���ȼ�����8����ռ��1����Ӧ

    RS485_Init(9600);						//��ʼ��RS485

    LED_PWMIO_Init();						//���Եơ��迹��������������

    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    delay_ms(1000);
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;

 

    FLASH_Working_Mode = STMFLASH_ReadHalfWord(FLASH_Running_Flag_ADDR);
    if(FLASH_Working_Mode == 0x1234)
    {
        LED1 = 1;
        delay_ms(1000);
        RS485_Sendbyte(APP1_Running);
        iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP1����

    }
    else if(FLASH_Working_Mode == 0x4567)
    {
        LED2 = 1;
        delay_ms(1000);
        RS485_Sendbyte(APP2_Running);
        iap_load_app(FLASH_APP2_ADDR);//ִ��FLASH APP2����

    }
    else
    {
        LED3 = 1;
        RS485_Sendbyte(IAP_Running);

    }



    while(1)
    {
        
        LED1 = 1;
        delay_ms(500);
        loop_Process();
        LED1 = 0;
        
        delay_ms(500);
        LED2 = 0;
    }
    

}



void loop_Process(void)																	//�����ϴ�����
{
    switch(Mode_Flag)
    {
        case Run_IAP:
            
            if(((*(vu32*)(FLASH_IAP_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
            {
                RS485_Sendbyte(IAP_ACK);
                FLASH_ErasePage(FLASH_APP1_ADDR);//�����������
                iap_load_app(FLASH_IAP_ADDR);//ִ��FLASH IAP����
            }  
		break;

		case Run_APP1:

            if(((*(vu32*)(0X20000a00+4))&0xFF000000) == 0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
            {
                
                RS485_Sendbyte(APP1_ACK); //
                iap_write_appbin(FLASH_APP1_ADDR, RxBuf, Rcv_Flag - 7);//����FLASH����

                if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000) == 0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
                {
                    
                    RS485_Sendbyte(APP1_Running); 
                    iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
                    
                }
   
            }
           
		break;

		case Run_APP2:
            if(((*(vu32*)(0X20000a00+4))&0xFF000000) == 0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
            {
                
                //RS485_Sendbyte(APP2_ACK); //
                iap_write_appbin(FLASH_APP2_ADDR, RxBuf, Rcv_Flag - 7);//����FLASH����

                if(((*(vu32*)(FLASH_APP2_ADDR + 4))&0xFF000000) == 0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
                {
                    
                    //RS485_Sendbyte(APP2_Running); 
                    iap_load_app(FLASH_APP2_ADDR);//ִ��FLASH APP����
                    
                }

                
            }
		break;

        case Load_APP:
            LED2 = 1;
            if(RxCnt > 1000)
            {   
                if(RxBuf[RxCnt - 6] == START_BYTE1&&
                    RxBuf[RxCnt - 5] == START_BYTE2&&
                    RxBuf[RxCnt - 4] == START_BYTE3)
                {
                   
                    bin_crc[0] = crc16(&RxBuf[RxCnt - 7], 5)>>8;
                    bin_crc[1] = (crc16(&RxBuf[RxCnt - 7], 5)&0xFF);

                    if(bin_crc[0] == RxBuf[RxCnt-2]&&
                       bin_crc[1] == RxBuf[RxCnt-1])
                    {
                        Mode_Flag = 0xff;
                     }
                    
                }
            }

                   
		break;

        case Acquire_software_Version:
            
		break;

		case Acquire_Data:
                       
        break;
        default:

        break;




    }

     
    
}

