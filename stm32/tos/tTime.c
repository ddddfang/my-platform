
#include "tinyOS.h"


static tList tTaskDelayedList; // 延时队列


void tTaskDelayedInit (void)
{
    tListInit(&tTaskDelayedList);
}

//---------utils-----------
void tTimeTaskWait (tTask * task, uint32_t ticks)
{
    task->delayTicks = ticks;
    tListAddLast(&tTaskDelayedList, &(task->delayNode)); // 将任务加入延时list中
    task->state |= TINYOS_TASK_STATE_DELAYED;
}

void tTimeTaskWakeUp (tTask * task)
{
    // 从延时list中删除,并清除delayed状态
    tListRemove(&tTaskDelayedList, &(task->delayNode));
    task->state &= ~TINYOS_TASK_STATE_DELAYED;
    task->delayTicks = 0;
    //fang: 这里不用 tTaskSchedRdy 一下 ??
}

void tTimeTaskRemove (tTask * task)
{
    //将延时的任务从延时队列中移除
    tListRemove(&tTaskDelayedList, &(task->delayNode));
}

//--------------------

void tTaskDelay (uint32_t delay)
{
    uint32_t status = tTaskEnterCritical();
 
    tTimeTaskWait(currentTask, delay);  // 设置延时值，插入延时队列
    tTaskSchedUnRdy(currentTask);       // 将任务从就绪表中移除

    // 选择处于ready状态的最高优先级的任务,并 trigger PendSV_Handler 进行任务切换
    tTaskSched();

    tTaskExitCritical(status); 
}

//tick 心跳,确保实时性的关键,唯一可能破坏这种保证的是 关中断
void tTaskSystemTickHandler (void)
{
    tNode * node;
    uint32_t status = tTaskEnterCritical();

    // update 所有有延时需求的任务的 delayTicks
    for (node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode) {
        tTask * task = tNodeParent(node, tTask, delayNode);
        if (--task->delayTicks == 0) {
            if (task->waitEvent) {  //如果任务还处于等待事件的状态
                tEventRemoveTask(task, (void *)0, tErrorTimeout);
            }
            tTimeTaskWakeUp(task);  // 将任务从延时队列中移除
            tTaskSchedRdy(task);    // 将任务恢复到就绪状态,这样下面 tTaskSched 就可能调度到它
        }
    }

    // update 当前任务的时间片(相同优先级任务靠时间片轮转)
    if (--currentTask->slice == 0) {
        if (tListCount(&taskTable[currentTask->prio]) > 0) {
            //如果只有当前一个任务的话,还是自己
            tListRemoveFirst(&taskTable[currentTask->prio]);
            tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));

            currentTask->slice = TINYOS_SLICE_MAX;  // 重置计数器
        }
    }
    tTaskExitCritical(status);

    // 选择处于ready状态的最高优先级的任务,并 trigger PendSV_Handler 进行任务切换
    tTaskSched();
}



