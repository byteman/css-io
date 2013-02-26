#include <intrins.h>

#include "datatypes.h"
#include "STC12C5A.H"
#include "protocal.h"

static volatile u8 send_ok = 1;
static volatile u8 recv_ok = 0;
static u8 recv_index = 0;
static u8 recv_buff[10];
static u8 send_buff[32];
static s8 send_count = 0;
static s8 send_len  =  0;
static Cmd gCmd;
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

/*********************************************** 
���ܣ�       �����ж� 	         
************************************************/
void UART_Interrupt() interrupt 4 using 2
{    
	static unsigned char send_count=0; 

	unsigned char buf;

  if(RI)
	{
		RI=0;
		buf = SBUF;

		if(buf == 0x02)
		{
			  recv_buff[0] = 0x2;
			  recv_index = 1;
			  recv_ok 	 = 0;
		}
		else if(buf == 0x03)
		{
			  if( recv_index == 5)
			  {
			  	  if( (recv_buff[0]+recv_buff[1]+recv_buff[2]+recv_buff[3]) ==  recv_buff[4])
			  	  {
			  	  	  recv_ok 	 = 1;
			  	  } 
			  }
				
				recv_index = 0;
		}
		else if(recv_index >= 1)
		{
			  if(recv_index < 4 )
			  {
			  		 recv_buff[recv_index++] = buf;
			  }	
		}        
	}  
	 	
  if(TI)
	{
    TI=0;
	 	send_count++;
	 	if(send_count >= send_len)
		{
        	send_count=0;
        	send_len = 0;
        	send_ok=1;
		}
		else SBUF=send_buff[send_count];
	}
}
/*********************************************** 
���ܣ�       ���ڷ�������	         
************************************************/
void UartSend(u8* sendbuf, u32 len)
{
	u32 i = 0;
	while(!send_ok);
	for(i = 0 ; i < len ; i++)
	{
		   send_buff[i] = sendbuf[i];
	}

	send_len = len;
	send_count = 0;
    SBUF=send_buff[0];	//��ʼ����
	
	send_ok=0;
}

Cmd* UartRecvOk()
{
	  if(recv_ok)
	  {
	  	gCmd.type = recv_buff[1];
	 	gCmd.dir = recv_buff[2];
	 	gCmd.param = recv_buff[3];
	 	recv_ok  = 0;
	 	return &gCmd;
	  }
	  return 0;
	 	
}