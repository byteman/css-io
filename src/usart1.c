#include <intrins.h>

#include "datatypes.h"
#include "STC12C5A.H"
#include "usart1.h"
#include "ByProtocol.h"
#include "TinyFifo.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_SEND_SIZE 128


//中断接收
//volatile bit uart1RxFlag = 0;

static volatile bit sendComplete   = 1;


///中断发送
static xdata char kd_string[MAX_SEND_SIZE];   //下面的两个buff的长度需要一致
static xdata u8 sendBuff[MAX_SEND_SIZE];
static xdata s8 sendCount = 0;
static xdata s8 sendLen   =  0;
static xdata u8 rx_char = 0;
static xdata u32 rx_count  = 0;
static xdata u32 tx_count  = 0;

/*!
	\brief 串口1初始化
*/
void UartInit(void)		//115200bps@22.1184MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	BRT = 0xF4;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
    ES = 1;
    EA = 1;
}
u32 getRxCount()
{
    return rx_count;
}
/*********************************************** 
功能：       串口中断 	         
************************************************/
void UART_Interrupt() interrupt 4 using 2
{    

    if(RI)
	{
		RI=0;
        rx_char = SBUF;
        rx_count++;
        tinyFifoPutc(rx_char);

	}  
	 	
    if(TI)
	{
        TI=0;
        tx_count++;
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
功能：       串口发送数据	         
************************************************/
void UartSend(u8* sendbuf, u32 len)
{
	xdata u32 i = 0;
	
    while(!sendComplete); //等待上一次发完数据

	for(i = 0 ; i < len ; i++)
	{
        sendBuff[i] = sendbuf[i];
	}

	sendLen     = len;
	sendCount   = 0;
    SBUF=sendBuff[0];	//开始发送
	
	sendComplete = 0;
}


void printk(const char* fmt,...)
{
	
	va_list ap;
	
	va_start(ap,fmt);
	
	memset(kd_string, 0, MAX_SEND_SIZE);
	vsprintf(kd_string, fmt, ap);

	UartSend((u8*)(&kd_string), strlen(kd_string));
		
	va_end(ap);
}