#ifndef BY_PROTOCOL_INCLUDE
#define BY_PROTOCOL_INCLUDE

#ifdef __cplusplus
extern "C"{
#endif


typedef unsigned char (*DataProcType) (unsigned char* context, unsigned int len);

void protoParserInit(DataProcType cbProc);

unsigned char parseChar(unsigned char rxChar);

unsigned int  buildPacket(unsigned char* context, unsigned int contextSize,
						  unsigned char* packet, unsigned int pktSize);



#ifdef __cplusplus
}
#endif

#endif
