#include "JDQ.H"
#include <stdio.h>
#include <assert.h>
/*
File
继电器管理模块:
Create @ 2008.12.06 by Byteman

*/

#ifdef __C51__
#define VAR_ATTR xdata
#else
#define VAR_ATTR 
#endif

///模块全局变量
static VAR_ATTR JDQ  jdq_con[MAX_JDQ_NUM];
static VAR_ATTR JDQWrite cbJdqFunc = NULL;
static VAR_ATTR JDQFilter cbJdqFilter = NULL;
static VAR_ATTR U8 JDQ_NUM = 0;
/////////////////////////////////

static void f_JDQ_Restore(U8 idx);
static void f_JDQ_DO(U8 idx,U8 status);

void       JDQ_Filter(JDQFilter filter)
{
	cbJdqFilter = filter;	
}
void      JDQ_Init(U8 numPort,JDQWrite cbJdq)
{
	VAR_ATTR U8 i = 0;
	JDQ_NUM = numPort;

	assert(JDQ_NUM <= MAX_JDQ_NUM);
	cbJdqFunc = cbJdq;
	for(i = 0; i < JDQ_NUM; i++)
	{
		jdq_con[i].who          = JDQ_NONE_ACTIVE;
		jdq_con[i].need_back    = 0;
		jdq_con[i].index        = i + 1;
		jdq_con[i].cur_status   = JDQ_STATUS_NC; //初始化所有继电器都接常闭触点
		jdq_con[i].pre_status   = JDQ_STATUS_NC;
		jdq_con[i].back_delay_S = 0;
		f_JDQ_DO(i,jdq_con[i].cur_status);//恢复到初始化状态
	}
	
}
/*
加强型控制:带延时恢复上次状态和恢复时有回调通知
采用该接口，有利于今后功能的扩展.
param:
	jdq_Par:控制参数，该结构可以扩展
*/
void JDQ_ControlEx(P_JDQ_CMD jdq_Par)
{
	VAR_ATTR U8 idx = jdq_Par->index;
	
	if((idx > JDQ_NUM) || (idx == 0))
		return;
	idx--;
	
	if(jdq_Par->status == jdq_con[idx].cur_status)//如果状态和当前一致
		return;	//直接返回，不控制	
	if(cbJdqFilter)
	{
		if(cbJdqFilter(jdq_Par->who,jdq_Par->index-1,jdq_Par->status))
		{
			return;
		}
	}

	jdq_con[idx].who              = jdq_Par->who;	
	
	//控制动作
	f_JDQ_DO(idx,jdq_Par->status);
	
	if(jdq_Par->need_back == 0) //如果不需要沿时输出
	{
		jdq_con[idx].who          = JDQ_NONE_ACTIVE;
	
		//保留上次的状态
		jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
		jdq_con[idx].cur_status   = jdq_Par->status;
		jdq_con[idx].need_back    = 0;
		jdq_con[idx].back_delay_S = 0;
		jdq_con[idx].jdq_Ex_Par   = NULL;
	}
	else
	{
		jdq_con[idx].who          = jdq_Par->who;
		//保留上次的状态
		jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
		jdq_con[idx].cur_status   = jdq_Par->status;
		jdq_con[idx].need_back    = jdq_Par->need_back;
		jdq_con[idx].back_delay_S = jdq_Par->back_S + 1;
		jdq_con[idx].jdq_Ex_Par   = (void*)jdq_Par->jdq_func;
	}
	
	
}

/*
普通控制指定序号的继电器
如果是反响控制的话去取消上一个继电器的控制命令
param:
	who:      谁(哪个模块)
	idx:      指定序号,从1开始
	status:   连接哪个触点 JDQ_STATUS_NC/JDQ_STATUS_NO
	
*/
void JDQ_Control(U8 who,U8 idx,U8 status)
{
	if((idx > JDQ_NUM) || (idx == 0))
		return;
	idx--;
	if(status == jdq_con[idx].cur_status)//如果状态和当前一致
		return;	//直接返回，不控制	
	if(cbJdqFilter)
	{
		if(cbJdqFilter(who,idx,status))
			return; //如果过滤驱动已经处理了该命令
	}

	jdq_con[idx].who         = who;	


	f_JDQ_DO(idx,status);
	
	//如果该继电器还处于延时恢复等待中,却要切换到另一种状态下....
	if((jdq_con[idx].need_back) && (jdq_con[idx].back_delay_S > 0))
	{
		//提前完成沿时恢复动作，并且通知
		if(jdq_con[idx].jdq_Ex_Par)
		{
			JDQ_CALLBACK_FUNC notify_func = jdq_con[idx].jdq_Ex_Par;
			notify_func(&jdq_con[idx]); //通知结束
		}
	}

	jdq_con[idx].who          = JDQ_NONE_ACTIVE;
	//保留上次的状态
	jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
	jdq_con[idx].cur_status   = status;
	jdq_con[idx].need_back    = 0;
	jdq_con[idx].back_delay_S = 0;
	jdq_con[idx].jdq_Ex_Par   = NULL;
		
}
/*
返回指定继电器的信息
*/
P_JDQ   JDQ_Info(U8 idx)
{
	if((idx > JDQ_NUM) || (idx == 0))
		return NULL;
	idx--;
	return (P_JDQ)&jdq_con[idx];
}
/*
返回所有继电器的状态信息 
*/
JDQ_STATUS JDQ_Status()
{
	VAR_ATTR JDQ_STATUS st;
	VAR_ATTR U8 i;
	st.value = 0;
	for(i = 0 ; i < JDQ_NUM; i++)
	{
		st.value |= (jdq_con[i].cur_status<<i);
	}
	return st;
}
/*
继电器管理模块服务函数
1s调用一次，在里面来检测继电器的动作
*/

void JDQ_Service(void)
{
	VAR_ATTR U8 i = 0;
	
	for(i = 0 ; i < JDQ_NUM; i++)
	{
		if(jdq_con[i].need_back) //继电器动作了
		{
			jdq_con[i].back_delay_S--;
			if(jdq_con[i].back_delay_S <= 0) //，延迟时间到了
			{
				f_JDQ_Restore(i);	
			}
		}
	}
}
/*
将恢复指定继电器成原来的状态 
param:
	idx:指定序号继电器
*/
static void f_JDQ_Restore(U8 idx)
{
	if( idx >= JDQ_NUM )
		return;
	//idx--;
	if(jdq_con[idx].pre_status == jdq_con[idx].cur_status)
		return;
	f_JDQ_DO(idx,jdq_con[idx].pre_status);
	
	jdq_con[idx].who          = JDQ_NONE_ACTIVE;
	jdq_con[idx].need_back    = 0; 
	jdq_con[idx].back_delay_S = 0;
	jdq_con[idx].cur_status   = jdq_con[idx].pre_status;
	
	if(jdq_con[idx].jdq_Ex_Par)
	{
		JDQ_CALLBACK_FUNC notify_func = (JDQ_CALLBACK_FUNC)jdq_con[idx].jdq_Ex_Par;
		notify_func(&jdq_con[idx]); //通知结束
	}
		
	jdq_con[idx].jdq_Ex_Par   = NULL;
	
	
}


//控制继电器动作
static void f_JDQ_DO(U8 idx,U8 status)
{
	//format_printf(APP_WARNING,"idx = %d,status = %d",idx,status);
	
	if( idx >= JDQ_NUM )
		return;
	if(cbJdqFunc)cbJdqFunc(idx,status);
	
}
