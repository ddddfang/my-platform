
#include "tinyOS.h"

//tTask * idleTask;   // 空闲任务 fang:看起来没用到 ????


//用于空闲任务的任务结构和堆栈空间
tTask tTaskIdle;
tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

void idleTaskEntry (void * param) {
    while (1)
    {
        // 暂时什么都不做,可以做的东西包括:
        //1.进入低功耗模式
        //2.刷新GUI
        //3.资源回收
        //4.状态统计
        //注意:此即优先级最低任务,不可以调用 os delay 进入挂起状态
        //因为cpu在所有高优先级任务都进入挂起时总得有点事做才行 !
    }
}

//int main () 
//{
//    tTaskSchedInit();
//
//    tTaskDelayedInit();
//
//    tInitApp();
//    
//    // 创建空闲任务
//    tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
//
//    // 第一次运行的时候, psp=0, 直接跳过前半段,后半段直接 currentTask=nextTask
//    nextTask = tTaskHighestReady();
//
//    tTaskRunFirst();    //这个函数永远不会返回
//    return 0;
//}

int tos_init ()
{
    tTaskSchedInit();
    tTaskDelayedInit();
    tInitApp();
    // 创建空闲任务
    tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
    return 0;
}

int tos_start ()
{
    // 第一次运行的时候, psp=0, 直接跳过前半段,后半段直接 currentTask=nextTask
    nextTask = tTaskHighestReady();
    tTaskRunFirst();    //这个函数永远不会返回
    return 0;
}



