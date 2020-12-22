
#include "tinyOS.h"
#include "led.h"
#include "systick.h"

//用于空闲任务的任务结构和堆栈空间
static tTask tTaskIdle;
static tTaskStack idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE];

extern void device_init(void);  //in main.c

void idleTaskEntry (void * param) {
    u32 i = 0;
    tInitApp();     // 创建任务
    device_init();

    LED_Set(LED0, Bit_RESET);
    LED_Set(LED1, Bit_SET);
    while (1)
    {
        // 暂时什么都不做,可以做的东西包括:
        //1.进入低功耗模式
        //2.刷新GUI
        //3.资源回收
        //4.状态统计
        //注意:此即优先级最低任务,不可以调用 os delay 进入挂起状态
        //因为cpu在所有高优先级任务都进入挂起时总得有点事做才行 !

        if (1 == systick_getval()) {
            if (++i > 50) {    //
                LED_Toggle(LED0);
                LED_Toggle(LED1);
                i = 0;
            }
        }
    }
}


int tos_init (void)
{
    tTaskSchedInit();
    tTaskDelayedInit();
    // 创建空闲任务
    tTaskInit(&tTaskIdle, idleTaskEntry, (void *)0, TINYOS_PRO_COUNT - 1, &idleTaskEnv[TINYOS_IDLETASK_STACK_SIZE]);
    return 0;
}

int tos_start (void)
{
    // 第一次运行的时候, psp=0, 直接跳过前半段,后半段直接 currentTask=nextTask
    nextTask = tTaskHighestReady();
    tTaskRunFirst();    //这个函数永远不会返回
    return 0;
}



