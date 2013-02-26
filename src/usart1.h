#ifndef USART1_H
#define USART1_H

void UartInit(void);
void UartSend(u8* sendbuf, u32 len);
Cmd* UartRecvOk();
#endif