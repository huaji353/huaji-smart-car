#include "set_task.h"

char _this_task = 0;//��ǰ���е�����ȫ�ֱ�־
char _task_for_cnt = 0;//ʹ��task_forʱ�������ʱ����
_task user_task[Task_Max_Num] = {0};//ÿ��Э�̵����м������

//����˵���������趨��ʱ����ʱ���׼����Ҫÿ��1ms(�Ƽ�ʱ��)����һ��
void set_task_mode(void)
{
	char task_num;//��ʱ����
	for(task_num = 0; task_num < Task_Max_Num; task_num++)//�������е���ʱ��
		if(user_task[task_num].time > 0)user_task[task_num].time--;//��Ҫ�Լ��ļ�1
}