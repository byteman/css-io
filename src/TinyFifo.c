#include "TinyFifo.h"
#include "stc12c5a.h"

#define MAX_RECV_SIZE 8
        
//������ֻ�ر��˴����жϣ���Ϊֻ�д����ж��е�����putc����������кܶ��ж��е��ã���ô��Ҫ�����ж�
#define ENTER_CRITICAL() ES=0   
#define LEAVE_CRITICAL() ES=1


static xdata u8 in = 0;
static xdata u8 out = 0;

static xdata u8 rxBuff[MAX_RECV_SIZE];
static volatile bit uart1RxFlag;

void tinyFifoPutc(u8 c)
{
    *(rxBuff + (in & (MAX_RECV_SIZE - 1))) = c;
	in++;

	if (in - out > MAX_RECV_SIZE)
		out++;
    uart1RxFlag = 1;
}
s8   tinyFifoGetc(u8* c)
{
    ENTER_CRITICAL();
    if (in == out)
    {
        uart1RxFlag = 0;
        LEAVE_CRITICAL();
		return -1;
    }
	*c = *(rxBuff + (out & (MAX_RECV_SIZE - 1)));
    out++;
    LEAVE_CRITICAL();
	return 0;
}

u8   tinyFifoLength()
{
	return (in - out);
}

u8   tinyFifoEmpty()
{
    return  uart1RxFlag?1:0;
}