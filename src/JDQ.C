#include "JDQ.H"
#include <stdio.h>
#include <assert.h>
/*
File
�̵�������ģ��:
Create @ 2008.12.06 by Byteman

*/

#ifdef __C51__
#define VAR_ATTR xdata
#else
#define VAR_ATTR 
#endif

///ģ��ȫ�ֱ���
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
		jdq_con[i].cur_status   = JDQ_STATUS_NC; //��ʼ�����м̵������ӳ��մ���
		jdq_con[i].pre_status   = JDQ_STATUS_NC;
		jdq_con[i].back_delay_S = 0;
		f_JDQ_DO(i,jdq_con[i].cur_status);//�ָ�����ʼ��״̬
	}
	
}
/*
��ǿ�Ϳ���:����ʱ�ָ��ϴ�״̬�ͻָ�ʱ�лص�֪ͨ
���øýӿڣ������ڽ���ܵ���չ.
param:
	jdq_Par:���Ʋ������ýṹ������չ
*/
void JDQ_ControlEx(P_JDQ_CMD jdq_Par)
{
	VAR_ATTR U8 idx = jdq_Par->index;
	
	if((idx > JDQ_NUM) || (idx == 0))
		return;
	idx--;
	
	if(jdq_Par->status == jdq_con[idx].cur_status)//���״̬�͵�ǰһ��
		return;	//ֱ�ӷ��أ�������	
	if(cbJdqFilter)
	{
		if(cbJdqFilter(jdq_Par->who,jdq_Par->index-1,jdq_Par->status))
		{
			return;
		}
	}

	jdq_con[idx].who              = jdq_Par->who;	
	
	//���ƶ���
	f_JDQ_DO(idx,jdq_Par->status);
	
	if(jdq_Par->need_back == 0) //�������Ҫ��ʱ���
	{
		jdq_con[idx].who          = JDQ_NONE_ACTIVE;
	
		//�����ϴε�״̬
		jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
		jdq_con[idx].cur_status   = jdq_Par->status;
		jdq_con[idx].need_back    = 0;
		jdq_con[idx].back_delay_S = 0;
		jdq_con[idx].jdq_Ex_Par   = NULL;
	}
	else
	{
		jdq_con[idx].who          = jdq_Par->who;
		//�����ϴε�״̬
		jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
		jdq_con[idx].cur_status   = jdq_Par->status;
		jdq_con[idx].need_back    = jdq_Par->need_back;
		jdq_con[idx].back_delay_S = jdq_Par->back_S + 1;
		jdq_con[idx].jdq_Ex_Par   = (void*)jdq_Par->jdq_func;
	}
	
	
}

/*
��ͨ����ָ����ŵļ̵���
����Ƿ�����ƵĻ�ȥȡ����һ���̵����Ŀ�������
param:
	who:      ˭(�ĸ�ģ��)
	idx:      ָ�����,��1��ʼ
	status:   �����ĸ����� JDQ_STATUS_NC/JDQ_STATUS_NO
	
*/
void JDQ_Control(U8 who,U8 idx,U8 status)
{
	if((idx > JDQ_NUM) || (idx == 0))
		return;
	idx--;
	if(status == jdq_con[idx].cur_status)//���״̬�͵�ǰһ��
		return;	//ֱ�ӷ��أ�������	
	if(cbJdqFilter)
	{
		if(cbJdqFilter(who,idx,status))
			return; //������������Ѿ������˸�����
	}

	jdq_con[idx].who         = who;	


	f_JDQ_DO(idx,status);
	
	//����ü̵�����������ʱ�ָ��ȴ���,ȴҪ�л�����һ��״̬��....
	if((jdq_con[idx].need_back) && (jdq_con[idx].back_delay_S > 0))
	{
		//��ǰ�����ʱ�ָ�����������֪ͨ
		if(jdq_con[idx].jdq_Ex_Par)
		{
			JDQ_CALLBACK_FUNC notify_func = jdq_con[idx].jdq_Ex_Par;
			notify_func(&jdq_con[idx]); //֪ͨ����
		}
	}

	jdq_con[idx].who          = JDQ_NONE_ACTIVE;
	//�����ϴε�״̬
	jdq_con[idx].pre_status   = jdq_con[idx].cur_status;
	jdq_con[idx].cur_status   = status;
	jdq_con[idx].need_back    = 0;
	jdq_con[idx].back_delay_S = 0;
	jdq_con[idx].jdq_Ex_Par   = NULL;
		
}
/*
����ָ���̵�������Ϣ
*/
P_JDQ   JDQ_Info(U8 idx)
{
	if((idx > JDQ_NUM) || (idx == 0))
		return NULL;
	idx--;
	return (P_JDQ)&jdq_con[idx];
}
/*
�������м̵�����״̬��Ϣ 
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
�̵�������ģ�������
1s����һ�Σ������������̵����Ķ���
*/

void JDQ_Service(void)
{
	VAR_ATTR U8 i = 0;
	
	for(i = 0 ; i < JDQ_NUM; i++)
	{
		if(jdq_con[i].need_back) //�̵���������
		{
			jdq_con[i].back_delay_S--;
			if(jdq_con[i].back_delay_S <= 0) //���ӳ�ʱ�䵽��
			{
				f_JDQ_Restore(i);	
			}
		}
	}
}
/*
���ָ�ָ���̵�����ԭ����״̬ 
param:
	idx:ָ����ż̵���
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
		notify_func(&jdq_con[idx]); //֪ͨ����
	}
		
	jdq_con[idx].jdq_Ex_Par   = NULL;
	
	
}


//���Ƽ̵�������
static void f_JDQ_DO(U8 idx,U8 status)
{
	//format_printf(APP_WARNING,"idx = %d,status = %d",idx,status);
	
	if( idx >= JDQ_NUM )
		return;
	if(cbJdqFunc)cbJdqFunc(idx,status);
	
}