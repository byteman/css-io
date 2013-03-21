
#include "common.h"
#include <intrins.h>

 //该函数可以有工具自动生成
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


unsigned int bit_reverse(unsigned int n)
{
	n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
	n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
	n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
	n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
	n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
	return n;
}

unsigned char revertBits(unsigned char in)
{
	
	xdata unsigned char out = 0;
	xdata unsigned char i = 0;

	for(i = 0; i < 8; i++)
	{
		if(in&0x1)
		{
			out |= (1 << (7-i));
		}
		in >>= 1;
	}

	return out;
	

}