/*************************************** Copyright (c)******************************************************
** File name            :   tinyOS.h
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS的核心头文件。包含了所有关键数据类型的定义，还有核心的函数。
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2016-06-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
**********************************************************************************************************/
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

// 当前任务：记录当前是哪个任务正在运行
extern tTask * currentTask;
// 下一个将即运行的任务：在进行任务切换前，先设置好该值，然后任务切换过程中会从中读取下一任务信息
extern tTask * nextTask;

uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);

void tTaskRunFirst (void); 
// 预先配置好currentTask和nextTask, 然后调用该函数
void tTaskSwitch (void);
// 获取当前最高优先级且可运行的任务
tTask * tTaskHighestReady (void);

void tTaskSchedInit (void);
// 禁止任务调度
void tTaskSchedDisable (void);
// 允许任务调度
void tTaskSchedEnable (void);

// 将任务设置为 Rdy 状态
void tTaskSchedRdy (tTask * task);
// 将任务设置为 UnRdy 状态
void tTaskSchedUnRdy (tTask * task);
// 将任务从就绪列表中移除
void tTaskSchedRemove (tTask * task);
// 选择下一个具体的任务，然后切换至该任务运行
void tTaskSched (void);

/**********************************************************************************************************
** Function name        :   tTimeTaskWait
** Descriptions         :   将任务加入延时队列中
** input parameters     :   task    需要延时的任务
**                          ticks   延时的ticks
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void tTimeTaskWait (tTask * task, uint32_t ticks);

/**********************************************************************************************************
** Function name        :   tTimeTaskWakeUp
** Descriptions         :   将延时的任务从延时队列中唤醒
** input parameters     :   task  需要唤醒的任务
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void tTimeTaskWakeUp (tTask * task);

/**********************************************************************************************************
** Function name        :   tTimeTaskRemove
** Descriptions         :   将延时的任务从延时队列中移除
** input parameters     :   task  需要移除的任务
** output parameters    :   无
** Returned value       :   无
***********************************************************************************************************/
void tTimeTaskRemove (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskDelay
** Descriptions         :   使当前任务进入延时状态。
** parameters           :   delay 延时多少个ticks
** Returned value       :   无
***********************************************************************************************************/
void tTaskDelay (uint32_t delay);

/**********************************************************************************************************
** Function name        :   tTaskSystemTickHandler
** Descriptions         :   系统时钟节拍处理。
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskSystemTickHandler (void);

// 任务相关信息结构
typedef struct _tTaskInfo {
    // 任务延时计数器
    uint32_t delayTicks;

    // 任务的优先级
    uint32_t prio;

    // 任务当前状态
    uint32_t state;

    // 当前剩余的时间片
    uint32_t slice;

    // 被挂起的次数
    uint32_t suspendCount;
}tTaskInfo;

/**********************************************************************************************************
** Function name        :   tTaskInit
** Descriptions         :   初始化任务结构
** parameters           :   task        要初始化的任务结构
** parameters           :   entry       任务的入口函数
** parameters           :   param       传递给任务的运行参数
** Returned value       :   无
***********************************************************************************************************/
void tTaskInit (tTask * task, void (*entry)(void *), void *param, uint32_t prio, uint32_t * stack);

/**********************************************************************************************************
** Function name        :   tTaskSuspend
** Descriptions         :   挂起指定的任务
** parameters           :   task        待挂起的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskSuspend (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskWakeUp
** Descriptions         :   唤醒被挂起的任务
** parameters           :   task        待唤醒的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskWakeUp (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskSetCleanCallFunc
** Descriptions         :   设置任务被删除时调用的清理函数
** parameters           :   task  待设置的任务
** parameters           :   clean  清理函数入口地址
** parameters           :   param  传递给清理函数的参数
** Returned value       :   无
***********************************************************************************************************/
void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param);

/**********************************************************************************************************
** Function name        :   tTaskForceDelete
** Descriptions         :   强制删除指定的任务
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskForceDelete (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskRequestDelete
** Descriptions         :   请求删除某个任务，由任务自己决定是否删除自己
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskRequestDelete (tTask * task);

/**********************************************************************************************************
** Function name        :   tTaskIsRequestedDelete
** Descriptions         :   是否已经被请求删除自己
** parameters           :   task  需要删除的任务
** Returned value       :   非0表示请求删除，0表示无请求
***********************************************************************************************************/
uint8_t tTaskIsRequestedDelete (void);

/**********************************************************************************************************
** Function name        :   tTaskDeleteSelf
** Descriptions         :   删除自己
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskDeleteSelf (void);

/**********************************************************************************************************
** Function name        :   tTaskGetInfo
** Descriptions         :   获取任务相关信息
** parameters           :   task 需要查询的任务
** parameters           :   info 任务信息存储结构
** Returned value       :   无
***********************************************************************************************************/
void tTaskGetInfo (tTask * task, tTaskInfo * info);

/**********************************************************************************************************
** Function name        :   tInitApp
** Descriptions         :   初始化应用接口
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tInitApp (void);


#endif /* TINYOS_H */
