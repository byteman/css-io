#ifndef USART1_H
#define USART1_H
#include "protocal.h"
void UartInit(void);
void UartSend(u8* sendbuf, u32 len);

int printk(const char* fmt,...);

u8 getChar();
u8 isRecvChar();

#endif