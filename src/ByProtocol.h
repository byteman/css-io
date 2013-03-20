#ifndef BY_PROTOCOL_INCLUDE
#define BY_PROTOCOL_INCLUDE

#ifdef __cplusplus
extern "C"{
#endif


typedef unsigned char (*DataProcType) (unsigned char* context, unsigned int len);

void protoParserInit(DataProcType cbProc);
///�������յ����ַ�����������˹涨��Э���򷵻�1������0
unsigned char  parseChar(unsigned char rxChar);
///��ȡ�Ѿ����յ������һ�ε����ݰ�
unsigned char* readPacket(unsigned int* pktLen);   
unsigned int   buildPacket(unsigned char* context, unsigned int contextSize,
						  unsigned char* packet, unsigned int pktSize);



#ifdef __cplusplus
}
#endif

#endif
