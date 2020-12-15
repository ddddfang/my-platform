
#ifndef TINYOS_H
#define TINYOS_H

#include "stm32f10x.h"
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY                   0
#define TINYOS_TASK_STATE_DESTROYED             (1 << 0)
#define TINYOS_TASK_STATE_DELAYED               (1 << 1)
#define TINYOS_TASK_STATE_SUSPEND               (1 << 2)

// Cortex-M 的堆栈单元类型
typedef uint32_t tTaskStack;


typedef struct _tTask {
    tTaskStack * stack;
    tNode linkNode;         // 连接结点
    uint32_t delayTicks;    // 任务延时计数器
    tNode delayNode;        // 延时结点：通过delayNode就可以将tTask放置到延时队列中
    uint32_t prio;          // 任务的优先级
    uint32_t state;         // 任务当前状态
    uint32_t slice;         // 当前剩余的时间片
    uint32_t suspendCount;  // 被挂起的次数
    void (*clean) (void * param);    // 任务被删除时调用的清理函数
    void * cleanParam;      // 传递给清理函数的参数
    uint8_t requestDeleteFlag;      // 请求删除标志，非0表示请求删除
} tTask;

// 任务相关信息
typedef struct _tTaskInfo {
    uint32_t delayTicks;    // 任务延时计数器
    uint32_t prio;
    uint32_t state;         // 任务当前状态
    uint32_t slice;         // 当前剩余的时间片
    uint32_t suspendCount;  // 被挂起的次数
} tTaskInfo;


//--------------------------------------------------
extern tTask * currentTask;
extern tTask * nextTask;

extern tList taskTable[TINYOS_PRO_COUNT];

uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);
void tTaskRunFirst();
void tTaskSwitch();

void tTaskInit (tTask * task, void (*entry)(void *), void *param, uint32_t prio, uint32_t * stack);

void tTaskSchedInit (void);
void tTaskSchedDisable (void);
void tTaskSchedEnable (void);
void tTaskSchedRdy (tTask * task);
void tTaskSchedUnRdy (tTask * task);
void tTaskSchedRemove (tTask * task);
tTask * tTaskHighestReady (void);
void tTaskSched (void);

void tTaskSuspend (tTask * task);
void tTaskWakeUp (tTask * task);

void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param);
void tTaskForceDelete (tTask * task);
void tTaskRequestDelete (tTask * task);
uint8_t tTaskIsRequestedDelete (void);
void tTaskDeleteSelf (void);

void tTaskGetInfo (tTask * task, tTaskInfo * info);



//--------------------------------------------------
void tTimeTaskWait (tTask * task, uint32_t ticks);
void tTimeTaskWakeUp (tTask * task);
void tTimeTaskRemove (tTask * task);

void tTaskDelayedInit (void);
void tTaskDelay (uint32_t delay);
void tTaskSystemTickHandler (void);




void tInitApp (void);

int tos_init ();
int tos_start ();

#endif /* TINYOS_H */
