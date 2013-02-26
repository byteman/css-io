#include <intrins.h>
#include "STC12C5A.H"

/*!
	\brief ����1��ʼ��
*/
void UartInit(void)		//115200bps@22.1184MHz
{
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
	BRT = 0xF4;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
}

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
	return 0;
}
