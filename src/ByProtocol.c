#include "ByProtocol.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define MAX_CONTEXT_SIZE 32
#define PAD_SIZE 3
#define KEY_HEADER	0xA3
/*
一种基于消息头的不定长度的协议解析函数,用于总数据长度小于255个字节
头部		: 0xA3
数据总长度  : 1 Bytes
长度的补码  : 1 Bytes (为了效率，可以不用)
数据内容    : n Bytes (不定长度,内容体可以自定义)
校验和	    : 1 Bytes(前面所有数据包括头部的和校验)
*/
typedef enum{
	STEP_HEADER=0,
	STEP_LENGTH,
	STEP_CHECK_LENGHT,
	STEP_DATA,
	STEP_CRC,
}StepParser;



typedef struct tagPacketParser{
	unsigned char header;
	unsigned char totalBytes;
	unsigned char context[MAX_CONTEXT_SIZE];
	unsigned char sum;

	unsigned char step;
	unsigned char curPos;
	unsigned char contextBytes;
}PacketParser;

static xdata PacketParser pktParser;
static void parserInit(unsigned char step);
static void calcSum(PacketParser* pPkt,unsigned char ch);
static DataProcType dataProcFunc = NULL;

void protoParserInit(DataProcType cbProc)
{
	dataProcFunc = cbProc;
	parserInit(STEP_HEADER);
}

void parserInit(unsigned char step)
{
	pktParser.header = KEY_HEADER;
	pktParser.step	 = step;
	pktParser.curPos = 0;
	pktParser.sum	 = 0;
}

void calcSum(PacketParser* pPkt,unsigned char ch)
{
	pPkt->sum += ch;
}
unsigned char checkSum(unsigned char* buf, unsigned int len)
{
	unsigned char sum = 0;
	unsigned int  i   = 0;
	for(; i < len; i++)
	{
		sum += buf[i];
	
	}
	return sum;
}
unsigned int  buildPacket(unsigned char* context, unsigned int contextSize,
						  unsigned char* packet, unsigned int pktSize)
{
	xdata unsigned int i = 0;

	assert(context && packet);
	assert (pktSize > (contextSize+PAD_SIZE));

	packet[i++] = KEY_HEADER;
	packet[i++] = contextSize+PAD_SIZE;
	//pakcet[i++] = (~pakcet[i-1]) + 1;
	memcpy(packet+i,context,contextSize);
	i += contextSize;
	packet[i++] = checkSum(packet,i);

	return i;
}

unsigned char parseChar(unsigned char rxChar)
{
    xdata unsigned char  ret = 0;
	switch(pktParser.step)
	{
		case STEP_HEADER:
			if(rxChar == pktParser.header)
			{
				pktParser.step++;
				pktParser.sum = rxChar;
			}
			break;
		case STEP_LENGTH:
			pktParser.totalBytes	 = rxChar;
			pktParser.contextBytes	 = pktParser.totalBytes-PAD_SIZE;
			calcSum(&pktParser,rxChar);
			//pktParser.step++;
			pktParser.step+=2; //跳过长度校验
			break;
		case STEP_CHECK_LENGHT:	
			if( ((~rxChar)+1) == pktParser.totalBytes)
			{
				pktParser.step++;
				calcSum(&pktParser,rxChar);
			}
			else //长度校验不通过，无效数据，从新开始接收
			{
				parserInit(STEP_HEADER);
			}
			break;
		case STEP_DATA:
			if(pktParser.curPos < pktParser.contextBytes)
			{
				pktParser.context[pktParser.curPos] = rxChar;
				pktParser.curPos++;
				calcSum(&pktParser,rxChar);
				if ( pktParser.curPos == pktParser.contextBytes )
				{
					pktParser.step++;
				}
			}
			else
			{
				parserInit(STEP_HEADER);
			}
			break;
		case STEP_CRC:
			//if(pktParser.sum == rxChar)
			if(1)
			{
				if(dataProcFunc)
				{
					if(dataProcFunc(pktParser.context,pktParser.contextBytes) == 0)
					{

					}

                    
                    
				}
				parserInit(STEP_HEADER);
				ret = 1;	
			}
			else
			{
				parserInit(STEP_HEADER);
			}
			break;
		default:
			parserInit(STEP_HEADER);
			break;
	}

	return ret;
	 	
}