#include "set_task.h"

char _this_task = 0;//当前运行的任务全局标志
char _task_for_cnt = 0;//使用task_for时所需的临时变量
_task user_task[Task_Max_Num] = {0};//每个协程的运行记忆变量

//函数说明：用于设定软定时器的时间基准，需要每隔1ms(推荐时间)调用一次
void set_task_mode(void)
{
	char task_num;//临时变量
	for(task_num = 0; task_num < Task_Max_Num; task_num++)//遍历所有的软定时器
		if(user_task[task_num].time > 0)user_task[task_num].time--;//需要自减的减1
}