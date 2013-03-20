#ifndef USART1_H
#define USART1_H
#include "protocal.h"
void UartInit(void);
void UartSend(u8* sendbuf, u32 len);

void printk(const char* fmt,...);

#endif