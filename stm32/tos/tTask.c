
#include "stm32f10x.h"
#include "tinyOS.h"

#define NVIC_INT_CTRL       0xE000ED04      // 中断控制及状态寄存器
#define NVIC_PENDSVSET      0x10000000      // 触发软件中断的值
#define NVIC_SYSPRI2        0xE000ED22      // 系统优先级寄存器
#define NVIC_PENDSV_PRI     0x000000FF      // 配置优先级

#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)

uint32_t tTaskEnterCritical (void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();        // CPSID I
    return primask;
}

void tTaskExitCritical (uint32_t status) {
    __set_PRIMASK(status);
}

void tTaskRunFirst()
{
    __set_PSP(0);   // 这里设置 PSP = 0, 用于与 tTaskSwitch()区分

    // 将 PendSV 的中断优先配置为最低, 这样只有在其它所有中断完成后, 才会触发该中断
    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  // trigger SysTick_Handler
    // will never reached here.
}

// 进行一次任务切换, 预先配置好 currentTask 和 nextTask
void tTaskSwitch()
{
    // 这个函数会在某个任务中调用，然后触发PendSV切换至其它任务
    // 之后的某个时候，将会再次切换到该任务运行，此时，开始运行该行代码, 返回到
    // tTaskSwitch调用处继续往下运行
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  // 向NVIC_INT_CTRL写NVIC_PENDSVSET，用于PendSV
}


void tTaskInit (tTask * task, void (*entry)(void *), void *param, uint32_t prio, uint32_t * stack)
{
    *(--stack) = (unsigned long)(1<<24);        // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stack) = (unsigned long)entry;          // 程序的入口地址
    *(--stack) = (unsigned long)0x14;           // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stack) = (unsigned long)0x12;           // R12, 未用
    *(--stack) = (unsigned long)0x3;            // R3, 未用
    *(--stack) = (unsigned long)0x2;            // R2, 未用
    *(--stack) = (unsigned long)0x1;            // R1, 未用
    *(--stack) = (unsigned long)param;          // R0 = param, 传给任务的入口函数
    *(--stack) = (unsigned long)0x11;           // R11, 未用
    *(--stack) = (unsigned long)0x10;           // R10, 未用
    *(--stack) = (unsigned long)0x9;            // R9, 未用
    *(--stack) = (unsigned long)0x8;            // R8, 未用
    *(--stack) = (unsigned long)0x7;            // R7, 未用
    *(--stack) = (unsigned long)0x6;            // R6, 未用
    *(--stack) = (unsigned long)0x5;            // R5, 未用
    *(--stack) = (unsigned long)0x4;            // R4, 未用

    task->slice = TINYOS_SLICE_MAX;             // 初始化任务的时间片计数
    task->stack = stack;                        // 保存最终的值
    task->prio = prio;                          // 设置任务的优先级
    task->state = TINYOS_TASK_STATE_RDY;        // 设置任务为就绪状态
    task->suspendCount = 0;                     // 初始挂起次数为0
    task->clean = (void(*)(void *))0;           // 设置清理函数
    task->cleanParam = (void *)0;               // 设置传递给清理函数的参数
    task->requestDeleteFlag = 0;                // 请求删除标记

    tNodeInit(&(task->delayNode));              // 初始化延时结点    
    tNodeInit(&(task->linkNode));               // 初始化链接结点
    tTaskSchedRdy(task);                        // 将任务插入就绪队列
}

// 挂起指定的任务
void tTaskSuspend (tTask * task)
{
    uint32_t status = tTaskEnterCritical();

    // 不允许对已经进入延时状态的任务挂起
    if (!(task->state & TINYOS_TASK_STATE_DELAYED)) {
        // 仅当该任务被执行第一次挂起操作时，才考虑是否要执行任务切换操作
        if (++task->suspendCount <= 1) {
            task->state |= TINYOS_TASK_STATE_SUSPEND;

            tTaskSchedUnRdy(task);      // 就是将其从就绪队列中移除
            if (task == currentTask) {  // 任务是自己, 就切换到其它任务
                tTaskSched();
            }
        }
    }
    tTaskExitCritical(status); 
}

// 唤醒被挂起的任务
void tTaskWakeUp (tTask * task)
{
    uint32_t status = tTaskEnterCritical();

    if (task->state & TINYOS_TASK_STATE_SUSPEND) {  // 检查任务是否处于挂起状态
        if (--task->suspendCount == 0) 
        {
            task->state &= ~TINYOS_TASK_STATE_SUSPEND;

            tTaskSchedRdy(task);    // 同时将任务放回就绪队列中
            tTaskSched();           // 唤醒过程中，可能有更高优先级的任务就绪，执行一次任务调度
        }
    }
    tTaskExitCritical(status); 
}

// 设置任务被删除时调用的清理函数
void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param) 
{
    task->clean = clean;
    task->cleanParam = param;
}

// 强制删除指定的任务
void tTaskForceDelete (tTask * task)
{
    uint32_t status = tTaskEnterCritical();

    if (task->state & TINYOS_TASK_STATE_DELAYED) {  // 如果任务处于延时状态，则从延时队列中删除
        tTimeTaskRemove(task);
    } else if (!(task->state & TINYOS_TASK_STATE_SUSPEND)) {
        // 如果任务不处于挂起状态，那么就是就绪态，从就绪表中删除
        tTaskSchedRemove(task);
    }

    if (task->clean) {  // 删除时，如果有设置清理函数，则调用清理函数
        task->clean(task->cleanParam);
    }

    // 如果删除的是自己，那么需要切换至另一个任务，所以执行一次任务调度
    if (currentTask == task) {
        tTaskSched();
    }

    tTaskExitCritical(status); 
}

// 请求删除某个任务，由任务自己决定是否删除自己
void tTaskRequestDelete (tTask * task)
{
    uint32_t status = tTaskEnterCritical();

    // 设置清除删除标记
    task->requestDeleteFlag = 1;

    tTaskExitCritical(status); 
}

// 是否已经被请求删除自己
// Returned value : 非0表示请求删除，0表示无请求
uint8_t tTaskIsRequestedDelete (void)
{
    uint8_t delete;

    uint32_t status = tTaskEnterCritical();

    // 获取请求删除标记
    delete = currentTask->requestDeleteFlag;

    tTaskExitCritical(status);
    return delete;
}

// 删除自己
void tTaskDeleteSelf (void)
{
    uint32_t status = tTaskEnterCritical();

    // 任务在调用该函数时, 必处于就绪状态, 只需要从就绪队列中移除即可
    tTaskSchedRemove(currentTask);

    // 删除时，如果有设置清理函数，则调用清理函数
    if (currentTask->clean) {
        currentTask->clean(currentTask->cleanParam);
    }
    tTaskSched();   // 接下来，肯定是切换到其它任务去运行

    tTaskExitCritical(status); 
}

// 获取任务相关信息
void tTaskGetInfo (tTask * task, tTaskInfo * info)
{
    uint32_t status = tTaskEnterCritical();

    info->delayTicks = task->delayTicks;                // 延时信息
    info->prio = task->prio;                            // 任务优先级
    info->state = task->state;                          // 任务状态
    info->slice = task->slice;                          // 剩余时间片
    info->suspendCount = task->suspendCount;            // 被挂起的次数

    tTaskExitCritical(status); 
}

