
#ifndef TINYOS_H
#define TINYOS_H

#include "stm32f10x.h"
#include "tLib.h"
#include "tConfig.h"

#define TINYOS_TASK_STATE_RDY                   0
#define TINYOS_TASK_STATE_DESTROYED             (1 << 0)
#define TINYOS_TASK_STATE_DELAYED               (1 << 1)
#define TINYOS_TASK_STATE_SUSPEND               (1 << 2)
#define TINYOS_TASK_WAIT_MASK                   (0xFF << 16)

// Cortex-M 的堆栈单元类型
typedef uint32_t tTaskStack;

typedef enum _tError {
    tErrorNoError = 0,      // 没有错误
    tErrorTimeout,          // 等待超时
    tErrorResourceUnavaliable,
    tErrorDel
} tError;

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
    struct _tEvent * waitEvent;     // 任务正在等待的事件类型
    void * eventMsg;                // 等待事件的消息存储位置
    uint32_t waitEventResult;       // 等待事件的结果
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
void tTaskRunFirst(void);
void tTaskSwitch(void);

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



//--------------------------------------------------
typedef enum  _tEventType {
    tEventTypeUnknown   = 0,    // 未知类型
    tEventTypeSem       = 1,    // 信号量类型
} tEventType;

typedef struct _tEvent {    // Event控制结构
    tEventType type;        // Event类型
    tList waitList;         // 任务等待列表
} tEvent;

void tEventInit (tEvent * event, tEventType type);
void tEventWait (tEvent *event, tTask *task, void *msg, uint32_t state, uint32_t timeout);
tTask * tEventWakeUp (tEvent *event, void *msg, uint32_t result);
void tEventRemoveTask (tTask *task, void *msg, uint32_t result);
uint32_t tEventWakeUpAll (tEvent * event, void * msg, uint32_t result);
uint32_t tEventWaitCount (tEvent * event);


//--------------------------------------------------
typedef struct _tSem {   // 信号量类型
    // 事件控制块, 该结构被特意放到起始处，以实现tSem同时是一个tEvent的目的
    tEvent event;
    uint32_t count;    // 当前的计数
    uint32_t maxCount;
} tSem;

typedef struct _tSemInfo {   // 信号量的信息
    uint32_t count;     // 当前信号量计数
    uint32_t maxCount;  // 信号量允许的最大计数
    uint32_t taskCount; // 当前等待的任务计数
} tSemInfo;

void tSemInit (tSem * sem, uint32_t startCount, uint32_t maxCount);
uint32_t tSemWait (tSem * sem, uint32_t waitTicks);
uint32_t tSemNoWaitGet (tSem * sem);
void tSemNotify (tSem * sem);
void tSemGetInfo (tSem * sem, tSemInfo * info);
uint32_t tSemDestroy (tSem * sem);


//--------------------------------------------------
void tInitApp (void);

int tos_init (void);
int tos_start (void);

#endif /* TINYOS_H */
