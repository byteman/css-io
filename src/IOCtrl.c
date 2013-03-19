#include <intrins.h>
#include "STC12C5A.H"
#include "IOCtrl.h"
#include "ByProtocol.h"
#include "usart1.h"
#include <stdio.h>

static void jdqCtrl(IOCmd* pCmd)
{
    char idx   = pCmd->param1;
    char state = pCmd->param2;
    char dir   = pCmd->dir;
    printk("enter %s\n","jdqCtrl");
    if( (idx < 8)  && (idx >= 0))
    {
      
        if(dir == DIR_SET)
        {
            if(state == JDQ_CLOSE)
                P1 &= (~(1<< idx));
            else
                P1 |= (1<< idx);
        }
    }
    else if(  pCmd->param1 >= 8 && (pCmd->param1 < 16))
    {
        idx -= 8;
        idx = 8-idx;

        if(dir == DIR_SET)
        {
            
            if(state == JDQ_CLOSE)
                P2 &= (~(1<< idx));
            else
                P2 |= (1<< idx);
        }
    }
}
static void jdqCtrlEx(IOCmd* pCmd)
{

}
static void adCtrl(IOCmd* pCmd)
{

}
static xdata IOCmd pCmdVar;

static xdata volatile u8 cmdOk = 0; 
unsigned char parseCmd(unsigned char* context, unsigned int len)
{
	
	if(context == NULL) return 0;

	pCmdVar.type    = context[0];
    pCmdVar.dir     = context[1];
    pCmdVar.param1  = context[2];
    pCmdVar.param2  = context[3];

    cmdOk = 1;
	return 1;
}
void ioCtrlSrv(void)
{
    if(cmdOk == 1)
    {
        int tp = pCmdVar.type;
        printk("type=%d\r\n",tp);
        switch(pCmdVar.type)
        {
            case DEV_TYPE_JDQ:
                jdqCtrl(&pCmdVar);
                break;
            default:
                break;
        }
        cmdOk = 0;
    }
}
void ioCtrlInit(void)
{
	protoParserInit(parseCmd);
}