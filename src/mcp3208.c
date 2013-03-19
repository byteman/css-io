#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "stc12c5a.h"

//======Òý½Å¶¨Òå======
sbit CS=P1^4;  
sbit DIN =P1^5;            
sbit DOUT =P1^6;
sbit SCLK=P1^7;

static u8  i  = 0;
static u16 ad = 0;

#define MASTER                  //define:master undefine:slave
//sfr SPSTAT      =   0xcd;       //SPI status register
#define SPIF        0x80        //SPSTAT.7
#define WCOL        0x40        //SPSTAT.6
//sfr SPCTL       =   0xce;       //SPI control register
#define SSIG        0x80        //SPCTL.7
#define SPEN        0x40        //SPCTL.6
#define DORD        0x20        //SPCTL.5
#define MSTR        0x10        //SPCTL.4
#define CPOL        0x08        //SPCTL.3
#define CPHA        0x04        //SPCTL.2
#define SPDHH       0x00        //CPU_CLK/4
#define SPDH        0x01        //CPU_CLK/16
#define SPDL        0x02        //CPU_CLK/64
#define SPDLL       0x03        //CPU_CLK/128
//sfr SPDAT       =   0xcf;       //SPI data register
sbit SPISS      =   P1^4;       //SPI slave select, connect to slave' SS(P1.4) pin


u8 SPISwap(u8 dat)
{
#ifdef MASTER
    SPISS = 0;                  //pull low slave SS
#endif
    SPDAT = dat;                //trigger SPI send
    while (!(SPSTAT & SPIF));   //wait send complete
    SPSTAT = SPIF | WCOL;       //clear SPI status
#ifdef MASTER
    SPISS = 1;                  //push high slave SS
#endif
    return SPDAT;               //return received SPI data
}

void Delay_uS(unsigned char us)
{
	for(;us>0;us--)
	{
		_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();	
	}
}

u16 sample(u8 chx)
{
    ad = 0;
    SCLK = 0;                          // clk low first
    CS = 1; //add byteman
    DIN = 1; Delay_uS(2);
    CS = 0;                          // chxip select

    DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // start bit

    DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // SGL/DIFF = 1

    

//    DIN = (chx&0x4)>>2; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2
	DIN = 0; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2
    //	DIN = (chx&0x4)>>2; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2

//    DIN = (chx&0x2)>>1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D1
	DIN = 0; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D1
    	//DIN = (chx&0x4)>>2; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2

//    DIN = (chx&0x1); Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D0
//	DIN = (chx&0x1); Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D0
   	DIN = 1; Delay_uS(2);SCLK = 0; Delay_uS(2);SCLK = 1;Delay_uS(2);      // D2

         

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

#if 0
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
