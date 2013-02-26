#include <intrins.h>
#include "datatypes.h"
#include "mcp3208.h"
#include "STC12C5A.H"

void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

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
	unsigned char cnt = 10;
	while(cnt--)
	{
		 Delay100ms();
	}
}
int main()
{
	Delay1S();
	UartInit();
	sample(1);
	
	return 0;
}
