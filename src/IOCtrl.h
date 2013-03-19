#ifndef IO_CTRL_INCLUDE
#define IO_CTRL_INCLUDE

#ifdef __cplusplus
extern "C"{
#endif



enum DeviceType
{
	DEV_TYPE_JDQ = 0,
	DEV_TYPE_JDQS,
	DEV_TYPE_AD
};

enum DeviceCtrlDir
{
	DIR_GET = 0,
	DIR_SET
};

enum JDQState{
	JDQ_CLOSE = 0,
	JDQ_OPEN
};

typedef struct {
	unsigned char type;
	unsigned char dir;
	unsigned char param1;
	unsigned char param2;
}IOCmd;


void ioCtrlInit(void);
void ioCtrlSrv(void);
#ifdef __cplusplus
}
#endif
#endif
