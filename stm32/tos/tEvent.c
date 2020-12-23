
#include "tinyOS.h"

void tEventInit (tEvent * event, tEventType type)
{
	event->type = type;
	tListInit(&event->waitList);
}

//fang:注意这个函数调用完毕必须要调用一下 tTaskSched()
//不然这个任务疯狂 tTaskEnterCritical & tTaskExitCritical
//根本不给定时器开中断调度其他任务的机会!
void tEventWait (tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout)
{
    uint32_t status = tTaskEnterCritical();
    task->state |= state;           // 标记任务处于等待某种事件的状态
    task->waitEvent = event;        // 设置任务等待的事件结构
    task->eventMsg = msg;           // 设置任务等待事件的消息存储位置
    task->waitEventResult = tErrorNoError;  // 清空事件的等待结果

    tTaskSchedUnRdy(task);                              // 将任务从就绪队列中移除
    tListAddLast(&event->waitList, &task->linkNode);    // 将任务插入到等待队列中

    // 如果发现有设置超时，在同时插入到延时队列中
    // 当时间到达时，由延时处理机制负责将任务从延时列表中移除，同时从事件列表中移除
    if (timeout) {
        tTimeTaskWait(task, timeout);
    }
    tTaskExitCritical(status); 
}

tTask * tEventWakeUp (tEvent *event, void *msg, uint32_t result)
{
    tNode *node;
    tTask *task = (tTask * )0;

    uint32_t status = tTaskEnterCritical();

    // 取出等待队列中的第一个结点, fang: only wake up the first ?? tEventRemoveAll
    if((node = tListRemoveFirst(&event->waitList)) != (tNode *)0) {
        task = (tTask *)tNodeParent(node, tTask, linkNode);

        // 设置收到的消息、结构，清除相应的等待标志位
        task->waitEvent = (tEvent *)0;
        task->eventMsg = msg;
        task->waitEventResult = result;
        task->state &= ~TINYOS_TASK_WAIT_MASK;

        // 任务申请了超时等待，这里检查下，将其从延时队列中移除
        if (task->delayTicks != 0) {
            tTimeTaskWakeUp(task);
        }
        // 将任务加入就绪队列
        tTaskSchedRdy(task);
    }
    tTaskExitCritical(status);
    return task;
}

//将任务从其等待队列中强制移除, result 告知事件的等待结果
void tEventRemoveTask (tTask *task, void *msg, uint32_t result)
{
    uint32_t status = tTaskEnterCritical();

    // 注意，这里没有检查waitEvent是否为空。既然是从事件中移除，那么认为就不可能为空
    tListRemove(&task->waitEvent->waitList, &task->linkNode);

    // 设置收到的消息、结构，清除相应的等待标志位
    task->waitEvent = (tEvent *)0;
    task->eventMsg = msg;
    task->waitEventResult = result;
    task->state &= ~TINYOS_TASK_WAIT_MASK;

    tTaskExitCritical(status);
}

//清除所有等待中的任务，将事件发送给所有任务, tEventWakeUpAll ?
uint32_t tEventWakeUpAll (tEvent * event, void * msg, uint32_t result)
{
    tNode  * node;
    uint32_t count;

    uint32_t status = tTaskEnterCritical();

    count = tListCount(&event->waitList);   // 获取等待中的任务数量

    while ((node = tListRemoveFirst(&event->waitList)) != (tNode *)0) {
        tTask * task = (tTask *)tNodeParent(node, tTask, linkNode);

        // 设置收到的消息、结构，清除相应的等待标志位
        task->waitEvent = (tEvent *)0;
        task->eventMsg = msg;
        task->waitEventResult = result;
        task->state &= ~TINYOS_TASK_WAIT_MASK;

        // 任务申请了超时等待，这里检查下，将其从延时队列中移除
        if (task->delayTicks != 0) {
            tTimeTaskWakeUp(task);
        }
        tTaskSchedRdy(task);
    }

    tTaskExitCritical(status);
    return  count;
}

//事件控制块中等待的任务数量
uint32_t tEventWaitCount (tEvent * event)
{
    uint32_t count = 0;
    uint32_t status = tTaskEnterCritical();

    count = tListCount(&event->waitList);

    tTaskExitCritical(status);
    return count;
}


