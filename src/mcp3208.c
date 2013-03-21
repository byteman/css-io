#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "stc12c5a.h"

//======Òý½Å¶¨Òå======
sbit CS=P1^4;  
sbit DIN =P1^5;            
sbit DOUT =P1^6;
sbit SCLK=P1^7;

static xdata u8  i  = 0;
static xdata u16 ad = 0;

//下次可以使用带内部AD的单片机,可以节约一个AD芯片 
void Delay_uS(unsigned char us)
{
	for(;us>0;us--)
	{
		_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();
        	_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();	
	}
}
#if 0      //for huameng
u16 sample(u8 chx)
{
    ad = 0;
    SCLK = 0;                          // clk low first
    CS = 1; //add byteman
    DIN = 1; Delay_uS(2);
    CS = 0;                          // chxip select

    DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // start bit

    DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // SGL/DIFF = 1

    

    DIN = (chx&0x4)>>2; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2
	//DIN = 0; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2
   
    DIN = (chx&0x2)>>1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D1
	//DIN = 0; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D1
   
    DIN = (chx&0x1); Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D0
   	//DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2

         

     SCLK = 0; Delay_uS(2);SCLK = 1; Delay_uS(2);SCLK = 0;Delay_uS(2);      // null
     _nop_ ();

     _nop_ ();
     if(DOUT==1) 
     {
        //SCLK = 1;
        CS   = 1;
        return 0xffff;
     }
    
    for (i=0;i<=11;i++) {              // 12 bit data

        Delay_uS(2);SCLK = 1; Delay_uS(2);SCLK = 0;Delay_uS(2);

        ad <<= 1;

        ad = ad | DOUT;

    }
    Delay_uS(2);SCLK = 0;Delay_uS(2);
    CS   = 1;

    return (ad);		
}
#else // for hitech 
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
     _nop_ ();

     _nop_ ();
     if(DOUT==1) 
     {
        //SCLK = 1;
        CS   = 1;
        return 0xffff;
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
#endif
