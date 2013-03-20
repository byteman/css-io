#ifndef _JDQ__H
#define _JDQ__H

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes.H"

#define  MAX_JDQ_NUM         16

//ACTIVE的定义:如果当前连接的触点和初始化的时候连接触点
//不一致就认为该继电器被激活

//定义谁现在在操作继电器
#define  JDQ_NONE_ACTIVE  0 //没有人在使用该继电器
#define  JDQ_HAND_ACTIVE  1 //手动操作了继电器
#define  JDQ_VIDEO_ACTIVE 2 //视频服务器管理操作了继电器
#define  JDQ_NET_ACTIVE   3 //网络故障操作了继电器
#define  JDQ_LAMP_ACTIVE  4 //路灯控制 
#define  JDQ_CAP_ACTIVE   5 //图像抓拍



typedef struct tag_JDQ_STATUS{
	unsigned jdq1 :1 ;
	unsigned jdq2 :1 ;
	unsigned jdq3 :1 ; 
	unsigned jdq4 :1 ;
	unsigned jdq5 :1 ;
	unsigned jdq6 :1 ; 
	unsigned jdq7 :1 ;
	unsigned jdq8 :1 ;
	unsigned jdq9 :1 ;
	unsigned jdq10 :1 ;
	unsigned jdq11 :1 ;
	unsigned jdq12 :1 ;
	unsigned jdq13 :1 ;
	unsigned jdq14 :1 ;
	unsigned jdq15 :1 ;
	unsigned jdq16 :1 ;
}__JDQ_STATUS;

typedef union
{
	__JDQ_STATUS jdq;
	U16          value;
}JDQ_STATUS;


/*
继电器对象管理结构体
*/
typedef struct tag_JDQ{
	 U8    who;         //继电器被谁在操作
	 U8    cur_status;  //操作后的状态
	 U8    pre_status;  //操作前的状态
	 U8    need_back;   //如果需要还原到原来的状态
	 U8    index;       //继电器序号
	 U8    back_delay_S;//还原延迟恢复时间
	 void* jdq_Ex_Par;  //延时恢复通知函数  
}JDQ,*P_JDQ;
/*
延时恢复通知函数原型
*/
typedef void (*JDQ_CALLBACK_FUNC)(P_JDQ jdq);
/*
继电器命令，对某个对应的继电器发出控制命令
*/
typedef struct tag_JDQ_CMD{
		U8  who;
		U8  index;
		U8  status;
		U8  need_back;
		U8  back_S;
		JDQ_CALLBACK_FUNC jdq_func;
}JDQ_CMD,*P_JDQ_CMD;

typedef enum {
	JDQ_STATUS_NO=0,//接常开触点
	JDQ_STATUS_NC//接常闭触点
}JDQState;
/*需要注册的继电器控制函数*/

typedef void (*JDQWrite)(U8 idx,U8 state);
typedef U8 (*JDQFilter)(U8 who,U8 idx,U8 state);
//初始化
extern void       JDQ_Init(U8 numPort,JDQWrite cbJdq);
//简单控制接口
extern void       JDQ_Control(U8 who,U8 idx,U8 status);
//扩展控制接口
extern void       JDQ_ControlEx(P_JDQ_CMD jdq_Par);
//返回指定继电器的信息
extern P_JDQ      JDQ_Info(U8 idx);
//返回指定所有继电器的当前状态
//extern JDQ_STATUS JDQ_Status(void);
extern JDQ_STATUS JDQ_Status(void); 
//服务函数
extern void       JDQ_Service(void);

//过滤函数
extern void       JDQ_Filter(JDQFilter filter);
#ifdef __cplusplus
}
#endif
#endif