#include <intrins.h>

#include "datatypes.h"
#include "STC12C5A.H"
#include "usart1.h"
#include "ByProtocol.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


static xdata u8 recv_index = 0;
static xdata u8 send_buff[128];
static xdata s8 send_count = 0;
static xdata s8 send_len  =  0;
static xdata volatile u8 send_ok   = 1;

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

#define MAX_LEN     (6)
#define LEFT_LEN (MAX_LEN-1)
/*********************************************** 
功能：       串口中断 	         
************************************************/
void UART_Interrupt() interrupt 4 using 2
{    
	static xdata unsigned char send_count=0; 

	unsigned char buf;

    if(RI)
	{
		RI=0;
		buf = SBUF;

	    if(parseChar(buf))
        {
            SBUF = 'G';
        }
	}  
	 	
    if(TI)
	{
        TI=0;
	 	send_count++;
	 	if(send_count >= send_len)
		{
        	send_count=0;
        	send_len = 0;
        	send_ok=1;
		}
		else SBUF=send_buff[send_count];
	}
}
/*********************************************** 
功能：       串口发送数据	         
************************************************/
void UartSend(u8* sendbuf, u32 len)
{
	xdata u32 i = 0;
	
    while(!send_ok);
	for(i = 0 ; i < len ; i++)
	{
        send_buff[i] = sendbuf[i];
	}

	send_len = len;
	send_count = 0;
    SBUF=send_buff[0];	//开始发送
	
	send_ok=0;
}

static xdata char kd_string[128];
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