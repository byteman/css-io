#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "stc12c5a.h"

//======�0�6�0�5�0�5�0�3�0�9���0�6�0�2======
sbit CS=P1^4;  
sbit DIN =P1^5;            
sbit DOUT =P1^6;
sbit SCLK=P1^7;

static xdata u8  i  = 0;
static xdata u16 ad = 0;
static xdata u8  cnt = 10;
//��Ҫ����ʱ�ӵ�Ƶ������������ת���õĵȴ�ʱ��
void wait(void)
{
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
     _nop_ ();
}
u16 sample(u8 chx)
{
    ad = 0;
    SCLK = 0;                          // clk low first
    CS = 1; //add byteman
    CS = 0;                          // chxip select

    DIN = 1; SCLK = 0; SCLK = 1;      // start bit

    DIN = 1; SCLK = 0; SCLK = 1;      // SGL/DIFF = 1

    
    
    DIN = (chx&0x4)>>2; SCLK = 0; SCLK = 1;      // D2

    DIN = (chx&0x2)>>1; SCLK = 0; SCLK = 1;      // D1

    DIN = (chx&0x1); SCLK = 0; SCLK = 1;      // D0

         

     SCLK = 0; SCLK = 1; SCLK = 0;      // null
     wait();

     if(DOUT==1) 
     {
        CS   = 1;
        return 0x0;  //�����û��ת���ã��ͱ��0
     }
    
    for (i=0;i<=11;i++) {              // 12 bit data

        SCLK = 1; SCLK = 0;

        ad <<= 1;

        ad = ad | DOUT;

    }
    SCLK = 0;
    CS   = 1;

    return (ad);	
		
}

