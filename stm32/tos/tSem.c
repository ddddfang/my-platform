
#include "tinyOS.h"

//初始化信号量, startCount 初始的计数, maxCount 最大计数，如果为0，则不限数量
void tSemInit (tSem * sem, uint32_t startCount, uint32_t maxCount)
{
    tEventInit(&sem->event, tEventTypeSem);

    sem->maxCount = maxCount;
    if (maxCount == 0) {
        sem->count = startCount;
    } else {
        sem->count = (startCount > maxCount) ? maxCount : startCount;
    }
}

//等待信号量,lock, waitTicks 当信号量计数为0时，等待的ticks数，为0时表示永远等待
uint32_t tSemWait (tSem * sem, uint32_t waitTicks)
{
    uint32_t status = tTaskEnterCritical();

    if (sem->count > 0) {   // 如果大于0的话，消耗掉一个，然后正常退出
        --sem->count;
        tTaskExitCritical(status);
        return tErrorNoError;
    } else {                // 将任务从就绪队列中移除,插入事件队列中
        tEventWait(&sem->event, currentTask, (void *)0,  tEventTypeSem, waitTicks);
        tTaskExitCritical(status);

        tTaskSched();   // 切换到其它任务
        // 当由于等待超时或者计数可用时，执行会返回到这里，然后取出等待结构
        return currentTask->waitEventResult;
    }
}

//获取信号量，如果信号量计数不可用，则立即退回
uint32_t tSemNoWaitGet (tSem * sem)
{
    uint32_t status = tTaskEnterCritical();

    if (sem->count > 0) {   // 如果大于0的话，消耗掉一个，然后正常退出
        --sem->count;
        tTaskExitCritical(status);
        return tErrorNoError;
    } else {                // 否则，返回资源不可用
        tTaskExitCritical(status);
        return tErrorResourceUnavaliable;
    }
}

//通知信号量可用,unlock，唤醒等待队列中的一个任务，或者将计数+1
void tSemNotify (tSem * sem)
{
    uint32_t status = tTaskEnterCritical();

    if (tEventWaitCount(&sem->event) > 0) { //如果有任务等待
        // 直接唤醒位于队列首部（最先等待）的任务
        tTask * task = tEventWakeUp(&sem->event, (void *)0, tErrorNoError);

        // 如果这个任务的优先级更高，就执行调度，切换过去
        if (task->prio < currentTask->prio) {
            tTaskSched();
        }
    } else {            // 如果没有任务等待的话，增加计数
        ++sem->count;
        if ((sem->maxCount != 0) && (sem->count > sem->maxCount)) {
            sem->count = sem->maxCount;
        }
    }
    tTaskExitCritical(status);
}

//查询信号量的状态信息
void tSemGetInfo (tSem * sem, tSemInfo * info)
{
    uint32_t status = tTaskEnterCritical();

    // 拷贝需要的信息
    info->count = sem->count;
    info->maxCount = sem->maxCount;
    info->taskCount = tEventWaitCount(&sem->event);

    tTaskExitCritical(status);
}

//销毁信号量, Returned value: 因销毁该信号量而唤醒的任务数量
uint32_t tSemDestroy (tSem * sem)
{
    uint32_t status = tTaskEnterCritical();

    // 清空事件控制块中的任务
    uint32_t count = tEventWakeUpAll(&sem->event, (void *)0, tErrorDel);
    sem->count = 0;
    tTaskExitCritical(status);

    if (count > 0) {    // 清空过程中可能有任务就绪，执行一次调度
        tTaskSched();
    }
    return count;
}

