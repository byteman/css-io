#include <intrins.h>
#include "STC12C5A.H"
#include "IOCtrl.h"
#include "ByProtocol.h"
#include "usart1.h"
#include "JDQ.h"
#include "datatypes.h"

#include <stdio.h>

extern void sendAD();
extern void adResumeSend();
extern void adStopSend();

static xdata  JDQ_CMD jdq_cmd;

static xdata u8 type  = 0;
static xdata u8 idx   = 0;
static xdata u8 state = 0;
static xdata u8 dir   = 0;
static xdata u8 ackBuf[32];

int sendIoAck(u8 cmd)
{
  
    u32 ll = 0;
    ll = buildPacket((u8*)&cmd,1,ackBuf,32);

    UartSend(ackBuf,11);

    //return UartSend((u8*)ackBuf,ll);
}
/*
指定控制某个继电器,LED指示灯和继电器是连接在一起的，所以不用单独控制LED和继电器的状态配合

继电器指定控制命令处理
    0xA3 :  head
    0x7  :  len
    0x00 :  DEV_TYPE_JDQ
    0x01 :  DIR_WRITE
    0x0  :  jdq index 
    0x0  :  jdq status   JDQ_STATUS_NO
    0xc  :  sum
A3 07 00 01 01 01 AD ==> set jdq[1] to      JDQ_STATUS_NO
A3 07 00 01 01 00 AC ==> set jdq[1] to      JDQ_STATUS_NC

A3 08 00 01 01 01 05 B3 ==> set jdq[1] to      JDQ_STATUS_NO for 5s  and return to JDQ_STATUS_NC

*/

static void jdqCtrl(IOCmd* pCmd,u8 argc)
{
    

    if(argc < 2) return;
    
    type  = pCmd->type;
    idx   = pCmd->param1;
    state = pCmd->param2;
    dir   = pCmd->dir;
  
    //printk("jdqCtrl type=%d dir=%d,idx=%d,state=%d\r\n",(int)type,(int)dir,(int)idx,(int)state);
    if(dir == DIR_SET)
    {
        jdq_cmd.who = 1;
        jdq_cmd.index = idx;
        jdq_cmd.status = state;     
        jdq_cmd.jdq_func = 0;
        if(type ==  DEV_TYPE_JDQ_TIME)
        {
            jdq_cmd.need_back = 1;
            if(argc < 3) 
                return;
            jdq_cmd.back_S = pCmd->param3;;
        }
        else if(type == DEV_TYPE_JDQ)
        {
            jdq_cmd.need_back = 0;
            jdq_cmd.back_S = 0;
        }
        JDQ_ControlEx(&jdq_cmd);
    }
}
/*!
继电器总体控制命令处理
    0xA3 :  head
    0x7  :  len
    0x01 :  DEV_TYPE_JDQS
    0x01 :  DIR_WRITE
    0x0  :  param1
    0x0  :  param2
    0xc  :  sum
A3 07 01 01 11 11 CE ==> set all jdqs
A3 07 01 00 11 11 CD ==> get all jdqs
*/
static void jdqCtrlEx(IOCmd* pCmd,u8 argc)
{

     if(argc < 2) return;

     P0 = pCmd->param1;
     P2 = (~pCmd->param2);

}
/*!
AD控制命令处理
    0xA3 :  head
    0x6  :  len
    0x03 :  DEV_TYPE_AD
    0x01 :  DIR_WRITE
    0x0  :  param1
    0xc  :  sum
A3 06 03 01 00 AD ==> stop  ad
A3 06 03 01 01 AD ==> resum ad
A3 06 03 01 02 AD ==> get   ad
*/
static void adCtrl(IOCmd* pCmd,u8 argc)
{
     if(argc < 1) return;

     switch(pCmd->param1)
     {
        case 0:
            adStopSend();
            break;
        case 1:
            adResumeSend();
            break;
        default:
            break;
     }
     sendAD();
     
}

unsigned char ioParsePacket( unsigned char* context, unsigned int len)
{
	
	if(context == NULL) return 0;

    if(len < 2)   return 0;

	switch(context[0])
    {
        case DEV_TYPE_JDQ:
        case DEV_TYPE_JDQ_TIME:
            jdqCtrl((IOCmd *)context,len-2);
            break;
        case DEV_TYPE_JDQS:
            jdqCtrlEx((IOCmd *)context,len-2);
            break;
        case DEV_TYPE_AD:
            adCtrl((IOCmd *)context,len-2);
            break;
        default:
            break;
    }

	return 1;
}

/*
对于具体产品的，具体定义继电器的NC和NO分别是高电平还是低电平
*/
void JDQWriteFunc(U8 idx,U8 state)
{

   // printk("jdq[%d] state[%d]\r\n",(int)idx,(int)state);
    
    if( (idx < 8)  && (idx >= 0))
    {
        if(state == JDQ_STATUS_NC)    
            P0 &= (~(1<< idx));
        else
            P0 |= (1<< idx); 
            
    }
    else if(  idx >= 8 && (idx < 16))
    {
        idx -= 7;
        idx = 8-idx;
        if(state == JDQ_STATUS_NC)
            P2 &= (~(1<< idx));
        else
            P2 |= (1<< idx);
    }
}

void ioCtrlInit(void)
{
	protoParserInit(NULL);
    JDQ_Init(16,JDQWriteFunc);
}