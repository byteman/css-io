#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "STC12C5A.H"
#include "usart1.h"
#include "IOCtrl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHANLE 		6
#define VER_MAJOR   1u
#define VER_MIN     0u


//红色的工作指示灯
sbit LED = P3^6;
#define RED_LED_OFF (LED=1)
#define RED_LED_ON  (LED=0)


static volatile flag_50ms = 0;
static volatile u8 timesFlag = 0;

static xdata s16 count = 2;
static xdata s16 count40 = 40;

static xdata int wildTicks = 0;
static xdata u32 wild_ticks_count = 0;  

static xdata u8   sendBuf[32];
static xdata u16	adCode[CHANLE];

static xdata Cmd *pCmd  = NULL;

void	readAD(void);

void Delay100ms()		//@11.0592MHz
{
	xdata unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 5;
	j = 52;
	k = 195;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay1S()
{
	xdata unsigned char cnt = 10;
	while(cnt--)
	{
		 Delay100ms();
	}
}

/* Timer0 interrupt routine */
void tm0_isr() interrupt 1 using 1
{
    TL0 = 0x00;              //reload timer0 low byte
    TH0 = 0x4C;                //reload timer0 high byte
    
    if (count-- == 0)               //50ms send
    {
        count = 2;               //reset counter
        readAD(); //50ms 采样一次
        flag_50ms = 1;
    }
    if( count40-- == 0)
    {
        count40 = 40;
        //LED = ~LED;
        timesFlag = 1;
	    wildTicks = wild_ticks_count; 
	    wild_ticks_count = 0;
    }
}
void Timer0Init(void)		//25毫秒@22.1184MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x4C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;                        //enable timer0 interrupt
    EA = 1;                         //open global interrupt switch
}
void	readAD(void)
{
	xdata unsigned char  chanlx;
	xdata unsigned short adcode = 0;
        
	for(chanlx = 0;chanlx<CHANLE;chanlx++)
	{		
		adCode[chanlx] = sample(chanlx);			
	}	
}
/************************************************
功能：发送AD   [17 bytes] 
header:             0xAA
幅度/仰角:          xx xx
高度                xx xx
重量                xx xx
单机模式编码器      xx xx
倾角传感器x         xx xx
倾角传感器y         xx xx
风速                xx xx
CHECKSUM            xx
TAIL:               0x55
************************************************/
void sendAD()		
{
	xdata unsigned char sum = 0;
	xdata int index = 0;
	xdata short tmp = 0;

	sendBuf[0] = 0xAA;
	for(index = 0; index < CHANLE;  index++)
	{
			sendBuf[index*2+1] = (adCode[index]>>8)&0xff;
			sendBuf[index*2+2] = adCode[index]&0xff;
	}   

	//if//if(times_flag) 1s 后才更新速度,因为风速是计算每秒采集的脉冲计数，所以必须等到1s后才能计算出速度
    if(timesFlag)
	{	    
			sendBuf[index*2+1] = (wildTicks>>8)&0x7f; // 15bit == 0 repr availbile
			sendBuf[index*2+2] = (wildTicks&0xff);
			timesFlag = 0;
            LED = ~LED;
	}else
    {
			sendBuf[index*2+1] = 0x80;      //15bit == 1 repr invalivble
			sendBuf[index*2+2] = 0x00;
	}
	for(index=1; index<=14; index++)
	{
		sum^=sendBuf[index];
	}
	sendBuf[15] = sum; 
	sendBuf[16] = 0x55; 
	
	UartSend(sendBuf,17);						
		 
}
void send_ad_data()
{
    if(flag_50ms)
    {
        sendAD();
        flag_50ms = 0;
    }	
}

void sendVersion(void)
{
    printk("ver%d.%d\r\n",VER_MAJOR,VER_MIN);
}


void PCA_isr() interrupt 7 using 1
{
    CCF0 = 0;                       //Clear interrupt flag
    LED = !LED;             //toggle the test pin while CEX0(P1.3) have a falling edge
    wild_ticks_count++;
}

void pcaInit()
{
    AUXR1 |= 0x40;                  //将PCA0切换到P4.2口
    CCON = 0;                       //Initial PCA control register
                                    //PCA timer stop running
                                    //Clear CF flag
                                    //Clear all module interrupt flag
    CL = 0;                         //Reset PCA base timer
    CH = 0;
    CMOD = 0x00;                    //Set PCA timer clock source as Fosc/12
                                    //Disable PCA timer overflow interrupt
    CCAPM0 = 0x11;                  //PCA module-0 capture by a negative tigger on CEX0(P1.3) and enable PCA interrupt
//  CCAPM0 = 0x21;                  //PCA module-0 capture by a rising edge on CEX0(P1.3) and enable PCA interrupt
//  CCAPM0 = 0x31;                  //PCA module-0 capture by a transition (falling/rising edge) on CEX0(P1.3) and enable PCA interrupt

    CR = 1;                         //PCA timer start run
    EA = 1;

    while (1);
}

void gpioInit(void)
{

    P1M1 = 0x40;
    P1M0 = 0xB0;
}


int main()
{
	Delay1S();
	UartInit();   //串口1初始化(115200 ,N8)
	Timer0Init();  //定时器0初始化(25ms中断一次)
    //pcaInit();     //PCA0做外部中断计数器
    gpioInit();
    ioCtrlInit();
	sendVersion(); //发送版本号

	while(1)
    {
         //Delay1S();
         //sendAD();  //发送采集的AD值
         ioCtrlSrv();
         
    }
	return 0;
}
