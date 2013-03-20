#ifndef _JDQ__H
#define _JDQ__H

#ifdef __cplusplus
extern "C" {
#endif

#include "DataTypes.H"

#define  MAX_JDQ_NUM         16

//ACTIVE�Ķ���:�����ǰ���ӵĴ���ͳ�ʼ����ʱ�����Ӵ���
//��һ�¾���Ϊ�ü̵���������

//����˭�����ڲ����̵���
#define  JDQ_NONE_ACTIVE  0 //û������ʹ�øü̵���
#define  JDQ_HAND_ACTIVE  1 //�ֶ������˼̵���
#define  JDQ_VIDEO_ACTIVE 2 //��Ƶ���������������˼̵���
#define  JDQ_NET_ACTIVE   3 //������ϲ����˼̵���
#define  JDQ_LAMP_ACTIVE  4 //·�ƿ��� 
#define  JDQ_CAP_ACTIVE   5 //ͼ��ץ��



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
�̵�����������ṹ��
*/
typedef struct tag_JDQ{
	 U8    who;         //�̵�����˭�ڲ���
	 U8    cur_status;  //�������״̬
	 U8    pre_status;  //����ǰ��״̬
	 U8    need_back;   //�����Ҫ��ԭ��ԭ����״̬
	 U8    index;       //�̵������
	 U8    back_delay_S;//��ԭ�ӳٻָ�ʱ��
	 void* jdq_Ex_Par;  //��ʱ�ָ�֪ͨ����  
}JDQ,*P_JDQ;
/*
��ʱ�ָ�֪ͨ����ԭ��
*/
typedef void (*JDQ_CALLBACK_FUNC)(P_JDQ jdq);
/*
�̵��������ĳ����Ӧ�ļ̵���������������
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
	JDQ_STATUS_NO=0,//�ӳ�������
	JDQ_STATUS_NC//�ӳ��մ���
}JDQState;
/*��Ҫע��ļ̵������ƺ���*/

typedef void (*JDQWrite)(U8 idx,U8 state);
typedef U8 (*JDQFilter)(U8 who,U8 idx,U8 state);
//��ʼ��
extern void       JDQ_Init(U8 numPort,JDQWrite cbJdq);
//�򵥿��ƽӿ�
extern void       JDQ_Control(U8 who,U8 idx,U8 status);
//��չ���ƽӿ�
extern void       JDQ_ControlEx(P_JDQ_CMD jdq_Par);
//����ָ���̵�������Ϣ
extern P_JDQ      JDQ_Info(U8 idx);
//����ָ�����м̵����ĵ�ǰ״̬
//extern JDQ_STATUS JDQ_Status(void);
extern JDQ_STATUS JDQ_Status(void); 
//������
extern void       JDQ_Service(void);

//���˺���
extern void       JDQ_Filter(JDQFilter filter);
#ifdef __cplusplus
}
#endif
#endif