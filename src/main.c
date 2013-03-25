#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "STC12C5A.H"
#include "usart1.h"
#include "IOCtrl.h"
#include "JDQ.h"     
#include "TinyFifo.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHANLE 		6
#define VER_MAJOR   1u
#define VER_MIN     0u
#define MAX_AD_BUFSIZE 32u


//红色的工作指示灯
sbit LED = P3^6;


////////////flag///////////////

static volatile bit jdqSrvFlag      = 0;
static volatile bit adSendFlag      = 0;
static volatile bit time50msFlag    = 0;
static volatile bit time25msFlag    = 0;
static volatile bit times1SFlag     = 0;

static xdata u32 wdgCount  = 0;

static xdata s16 count50MS = 2;
static xdata s16 count1S   = 40;
static xdata u16 wildTicks = 0;
static xdata u16 wildTicksCount = 0;  

static xdata u8  sendBuf[MAX_AD_BUFSIZE];
static xdata u16 adCode[CHANLE];

static xdata u32 pktLen  = 0;
static xdata u8* pkt = NULL;


static void	readAD(void);


/* Timer0 interrupt routine */
void tm0_isr() interrupt 1 using 1
{
    TL0 = 0x00;              //reload timer0 low byte
    TH0 = 0x4C;                //reload timer0 high byte
    time25msFlag = 1;
    if (count50MS-- == 0)               //50ms send
    {
        count50MS = 2;               //reset counter
        
        time50msFlag = 1;
    }
    if( count1S-- == 0)
    {
        count1S = 40;
        //LED = ~LED;
        times1SFlag = 1;
        jdqSrvFlag  = 1;
	    wildTicks = wildTicksCount; 
	    wildTicksCount = 0;
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
void Delay100us()		//@22.1184MHz
{
	unsigned char i, j;

	i = 3;
	j = 35;
	do
	{
		while (--j);
	} while (--i);
}

xdata unsigned char  chanlx;
void	readAD(void)
{      
	for(chanlx = 0;chanlx<CHANLE;chanlx++)
	{		
		adCode[chanlx] = sample(chanlx);
        Delay100us();
        			
	}	
}
/************************************************

功能：发送AD   [17 bytes] 
header:             0xAA
幅度/仰角:          xx xx
高度                xx xx
重量                xx xx
//单机模式编码器      xx xx [这个暂时不用，换成继电器的状态]
继电器状态          xx xx
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

	sendBuf[0] = 0xAA;
    readAD(); //50ms 采样一次
	for(index = 0; index < CHANLE;  index++)
	{
		sendBuf[index*2+1] = (adCode[index]>>8)&0xff;
		sendBuf[index*2+2] = adCode[index]&0xff;
	}   
	sendBuf[7] = ~P0;
	sendBuf[8] = ~ReverseBits(P2);
	//if//if(times_flag) 1s 后才更新速度,因为风速是计算每秒采集的脉冲计数，所以必须等到1s后才能计算出速度
    if(times1SFlag)
	{	    
		sendBuf[index*2+1] = (wildTicks>>8)&0x7f; // 15bit == 0 repr availbile
		sendBuf[index*2+2] = (wildTicks&0xff);
		times1SFlag = 0;
	}else
    {
		sendBuf[index*2+1] = 0x80;      //15bit == 1  invalid
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
void adStopSend()
{
    adSendFlag = 0;
}
void adResumeSend()
{
    adSendFlag = 1;
}
void sendADSrv()
{
    
    if(time50msFlag && adSendFlag)
    {
        
        sendAD();
        time50msFlag = 0;
    }	
}

void sendVersion(void)
{
    printk("huameng io board ver%d.%d\r\n",VER_MAJOR,VER_MIN);
}

void PCA_isr() interrupt 7 using 1
{
    CCF0 = 0;                       //Clear interrupt flag
    LED = ~LED;             //toggle the test pin while CEX0(P1.3) have a falling edge
    wildTicksCount++;
}
//做脉冲捕获用
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

}
///启用推挽输出
void gpioInit(void)
{

    P1M1 = 0x40;        //0100 0000
    P1M0 = 0xB0;        //1011 0000

    P0M1 = 0x00;        //0100 0000
    P0M0 = 0xFF;        //1011 0000

    P2M1 = 0x00;        //0100 0000
    P2M0 = 0xFF;        //1011 0000


}


void watchDogInit()     // 1.137s 
{ 
     WDT_CONTR |= 0x5;  //prev_div=3 (timeout = ( 12 * 2^(prev_div+1) * 32768 ) /  OSC_FREQ = 1.137s)
     WDT_CONTR |= 0x8 ; //enable idle count
     WDT_CONTR |= 0x20; //enable dog
     WDT_CONTR |= 0x10; //clear dog  
}
void watchDogReset()
{
    WDT_CONTR |= 0x10; //clear dog 
}

xdata u8 rxChar = 0;
int main()
{
   
	Delay100ms();
	
	UartInit();     //串口1初始化(115200 ,N8)
    pcaInit();    //PCA0做外部中断计数器
    gpioInit();     //启用推挽模式
    ioCtrlInit();   //通信协议初始化
	sendVersion();  //发送版本号

    Timer0Init();  //定时器0初始化(25ms中断一次)
    //watchDogInit(); //启动看门狗
	while(1)
    {
         sendADSrv();
         #if 1
         if(!tinyFifoEmpty())
         {
            if(tinyFifoGetc(&rxChar) == 0) //get data ok;
            {
                //printk("0x%0x\r\n",rxChar);
                if(parseChar(rxChar))
                {
                    pkt = readPacket(&pktLen);
                    if(pkt) ioParsePacket(pkt,pktLen);
                } 
            }
            
         }
         #endif
         if(jdqSrvFlag) // 1s call
         {
              JDQ_Service();
              //printk("wildcnt=%d\r\n",wildTicksCount);
              jdqSrvFlag = 0;
         }

         //if((( wdgCount++ ) % 10000 ) == 0) watchDogReset();    //watch dog


         
    }
	return 0;
}
