#ifndef PROTOCAL_H
#define PROTOCAL_H
#include "datatypes.h"

#define TYPE_JDQ_INDEX ('3')
#define TYPE_JDQ ('2')
#define TYPE_AD  ('1')
#define DIR_WRITE ('1')
#define DIR_READ ('0')

typedef struct{
	 u8 type;
	 u8 dir;
	 u8 param1;
     u8 param2;
}Cmd;

#endif
