
#include "common.h"
#include <intrins.h>

 //�ú��������й����Զ�����
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
