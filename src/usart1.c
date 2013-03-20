#include <intrins.h>

#include "datatypes.h"
#include "STC12C5A.H"
#include "usart1.h"
#include "ByProtocol.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_SEND_SIZE 128
#define MAX_RECV_SIZE 8
//�жϽ���
static volatile bit uart1RxFlag = 0;

static xdata u8 rxChar  = 0;
//static xdata u8 rxCount = 0;
//static xdata u8 rxBuff[MAX_RECV_SIZE];

///�жϷ���
static xdata u8 sendBuff[MAX_SEND_SIZE];
static xdata s8 sendCount = 0;
static xdata s8 sendLen   =  0;
static volatile bit sendComplete   = 1;

static xdata char kd_string[MAX_SEND_SIZE];

u8 isRecvChar()
{
    return  uart1RxFlag?1:0;
}
u8 getChar()
{
    uart1RxFlag = 0;
    return  rxChar;
}
/*!
	\brief ����1��ʼ��
*/
void UartInit(void)		//115200bps@22.1184MHz
{
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xF4;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
    ES = 1;
    EA = 1;
}

/*********************************************** 
���ܣ�       �����ж� 	         
************************************************/
void UART_Interrupt() interrupt 4 using 2
{    

    if(RI)
	{
		RI=0;
		rxChar = SBUF;
        uart1RxFlag = 1;
	}  
	 	
    if(TI)
	{
        TI=0;
	 	sendCount++;
	 	if(sendCount >= sendLen)
		{
        	sendCount=0;
        	sendLen = 0;
        	sendComplete = 1;
		}
		else SBUF=sendBuff[sendCount];
	}
}
/*********************************************** 
���ܣ�       ���ڷ�������	         
************************************************/
void UartSend(u8* sendbuf, u32 len)
{
	xdata u32 i = 0;
	
    while(!sendComplete);
	for(i = 0 ; i < len ; i++)
	{
        sendBuff[i] = sendbuf[i];
	}

	sendLen = len;
	sendCount = 0;
    SBUF=sendBuff[0];	//��ʼ����
	
	sendComplete = 0;
}


int printk(const char* fmt,...)
{
	
	va_list ap;
	
	va_start(ap,fmt);
	
	memset(kd_string, 0, 64);
	vsprintf(kd_string, fmt, ap);

	UartSend((u8*)(&kd_string), strlen(kd_string));
		
	va_end(ap);
	
	return 1;
}