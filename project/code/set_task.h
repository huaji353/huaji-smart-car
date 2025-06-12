#ifndef __SET_TASK_H
#define __SET_TASK_H

#define Task_Max_Num 10 // 最大任务数量，后面调用task_start的时候不可超过这个数字
// 建议按照需求来，太多了会降低性能

// 每个任务的数据结构
typedef struct
{
	unsigned int time;
	unsigned long state;
} _task;

extern char _this_task;    // 当前运行的任务全局标志
extern char _task_for_cnt; // 使用task_for时所需的临时变量
extern _task user_task[Task_Max_Num]; // 每个协程的运行记忆变量

void set_task_mode(void);
// 用于设置定时器的时间基准，需要每1ms调用一次（推荐），可以设置为其他基准
// 如果设置为1s调用一次，那么task_delay(1)就变成1s的延迟了，以此类推

#define task_start(task_num) _this_task=task_num;if(task_num<Task_Max_Num)switch(user_task[_this_task].state){case 0:
// 初始化线程，调用前必备

#define task_wait(ifx) user_task[_this_task].state=__LINE__;case __LINE__:if(ifx)break;
// 是否继续向下，内部填入表达式，为1则等待，为0则不等待

#define task_delay(delay_ms) user_task[_this_task].state=__LINE__;\
user_task[_this_task].time=delay_ms;case __LINE__:if(user_task[_this_task].time!=0)break;
// 延迟后继续向下，内部填入延迟ms时间，等待时间后继续向下

#define task_for(init, cnt) init;user_task[_this_task].state = __LINE__;\
case __LINE__: for(_task_for_cnt = 1;_task_for_cnt--;cnt,user_task[_this_task].state = __LINE__)
// 传入两个表达式，init是初始化(仅调用一次)，cnt是自增自减表达式(每次运行都调用一次(task_for_end返回来也算))

#define task_while() user_task[_this_task].state = __LINE__;\
case __LINE__: for(_task_for_cnt = 1;_task_for_cnt--;user_task[_this_task].state = __LINE__)
// while循环，内部不可以填，判断条件通过task_break进行判断

#define task_break(ifx) if(ifx)break;
// 对ifx的条件进行判断，为1则回到task_for/task_while的开始，为0则往下继续执行

#define task_end(reload) if(reload)user_task[_this_task].state=0;else user_task[_this_task].state = __LINE__;break;}
// 线程结束，内部填入是否重新开始线程，1为重新开始，0为结束等待

#endif