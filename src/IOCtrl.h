#ifndef IO_CTRL_INCLUDE
#define IO_CTRL_INCLUDE

#ifdef __cplusplus
extern "C"{
#endif


#include "ByProtocol.h"

enum DeviceType
{
	DEV_TYPE_JDQ = 0,
	DEV_TYPE_JDQS,
    DEV_TYPE_JDQ_TIME,
	DEV_TYPE_AD
};

enum DeviceCtrlDir
{
	DIR_GET = 0,
	DIR_SET
};


typedef struct {
	unsigned char type;
	unsigned char dir;
	unsigned char param1;
	unsigned char param2;
    unsigned char param3;
}IOCmd;


void ioCtrlInit(void);
unsigned char ioParsePacket(unsigned char* context, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif
