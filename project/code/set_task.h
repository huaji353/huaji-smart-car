#ifndef __SET_TASK_H
#define __SET_TASK_H

#define Task_Max_Num 10 // ��������������������task_start��ʱ�򲻿ɳ����������
// ���鰴����������̫���˻ή������

// ÿ����������ݽṹ
typedef struct
{
	unsigned int time;
	unsigned long state;
} _task;

extern char _this_task;    // ��ǰ���е�����ȫ�ֱ�־
extern char _task_for_cnt; // ʹ��task_forʱ�������ʱ����
extern _task user_task[Task_Max_Num]; // ÿ��Э�̵����м������

void set_task_mode(void);
// �������ö�ʱ����ʱ���׼����Ҫÿ1ms����һ�Σ��Ƽ�������������Ϊ������׼
// �������Ϊ1s����һ�Σ���ôtask_delay(1)�ͱ��1s���ӳ��ˣ��Դ�����

#define task_start(task_num) _this_task=task_num;if(task_num<Task_Max_Num)switch(user_task[_this_task].state){case 0:
// ��ʼ���̣߳�����ǰ�ر�

#define task_wait(ifx) user_task[_this_task].state=__LINE__;case __LINE__:if(ifx)break;
// �Ƿ�������£��ڲ�������ʽ��Ϊ1��ȴ���Ϊ0�򲻵ȴ�

#define task_delay(delay_ms) user_task[_this_task].state=__LINE__;\
user_task[_this_task].time=delay_ms;case __LINE__:if(user_task[_this_task].time!=0)break;
// �ӳٺ�������£��ڲ������ӳ�msʱ�䣬�ȴ�ʱ����������

#define task_for(init, cnt) init;user_task[_this_task].state = __LINE__;\
case __LINE__: for(_task_for_cnt = 1;_task_for_cnt--;cnt,user_task[_this_task].state = __LINE__)
// �����������ʽ��init�ǳ�ʼ��(������һ��)��cnt�������Լ����ʽ(ÿ�����ж�����һ��(task_for_end������Ҳ��))

#define task_while() user_task[_this_task].state = __LINE__;\
case __LINE__: for(_task_for_cnt = 1;_task_for_cnt--;user_task[_this_task].state = __LINE__)
// whileѭ�����ڲ���������ж�����ͨ��task_break�����ж�

#define task_break(ifx) if(ifx)break;
// ��ifx�����������жϣ�Ϊ1��ص�task_for/task_while�Ŀ�ʼ��Ϊ0�����¼���ִ��

#define task_end(reload) if(reload)user_task[_this_task].state=0;else user_task[_this_task].state = __LINE__;break;}
// �߳̽������ڲ������Ƿ����¿�ʼ�̣߳�1Ϊ���¿�ʼ��0Ϊ�����ȴ�

#endif