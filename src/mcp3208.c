//static uint value = 0;
#include	"include.h"
#include	"CS5532DEF.h"

//======Òý½Å¶¨Òå======
sbit CS=P1^2;  
sbit DIN =P1^5;            
sbit DOUT =P1^6;
sbit SCLK=P1^7;
unsigned short  sample(unsigned char chx)
{

	unsigned char i;

    unsigned int x;

    x = 0;

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

        x <<= 1;

        x = x | DOUT;

    }
    SCLK = 0;
    CS   = 1;
    //CS   = 0;

    return (x);	
		
}
