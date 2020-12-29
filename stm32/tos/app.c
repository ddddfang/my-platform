
#include "tinyOS.h"
#include "led.h"
#include "usart.h"

// 任务1和任务2的任务结构，以及用于堆栈空间
static tTask tTask1;
static tTask tTask2;
static tTask tTask3;
//static tTask tTask4;
static tTaskStack task1Env[1024];
static tTaskStack task2Env[1024];
static tTaskStack task3Env[1024];
//static tTaskStack task4Env[1024];
//
//static tEvent eDataReady;
//static tSem sem1;
//tSem sem2;
static tMbox mbox;
static void *mboxMsgBuffer[20];
static uint32_t msg[20];

void task1Entry (void * param) 
{
    while(!tTaskIsRequestedDelete()) {

        //generate data here..
        uart_putchar('1');
        uart_putchar(':');
        tTaskDelay(100); //100*10ms
        //data is ready now..

        //tSemNotify (&sem1);

        //uint32_t wakeup_cnt = tEventWakeUpAll(&eDataReady, (void *)0, 0);
        //if (wakeup_cnt > 0) {
        //    tTaskSched();
        //}

        for (uint32_t i = 0; i < 20; i++) {
            //msg is a uint32_t.
            msg[i] = i;
            if (tMboxNotify(&mbox, &msg[i], tMBOXSendNormal) == tErrorResourceFull) {
                uart_putchar('x');
            }
        }
        uart_putchar('\r');
        uart_putchar('\n');
    }
}

void task2Entry (void * param)
{
    while(!tTaskIsRequestedDelete()) {
        //uint32_t status = tSemWait(&sem1, 0);
        //tEventWait (&eDataReady, currentTask, (void *)0, 0, 0);
        //tTaskSched();
        uart_putchar('2');
        uart_putchar(':');
        void * msg;
        uint32_t value;
        uint32_t err = tMboxWait(&mbox, &msg, 1000);
        if (err == tErrorNoError) {
            value = *(uint32_t *)msg;
            uart_putchar('A'+value);
        }
        tTaskDelay(1);
        uart_putchar('\r');
        uart_putchar('\n');
    }
}

void task3Entry (void * param)
{
    while(!tTaskIsRequestedDelete()) {
        //uint32_t status = tSemWait(&sem2, 0);
        //tEventWait (&eDataReady, currentTask, (void *)0, 0, 0);
        //tTaskSched();
        uart_putchar('3');
        uart_putchar(':');
        void * msg;
        uint32_t value;
        uint32_t err = tMboxWait(&mbox, &msg, 100);
        if (err == tErrorNoError) {
            value = *(uint32_t *)msg;
            uart_putchar('a'+value);
        }
        tTaskDelay(1);
        uart_putchar('\r');
        uart_putchar('\n');
    }
}


void tInitApp (void)   //idleTask in tos.c will call it and init devices
{
    //tEventInit (&eDataReady, tEventTypeUnknown);
    //tSemInit(&sem1, 0, 0);
    //tSemInit(&sem2, 0, 10);
    tMboxInit(&mbox, mboxMsgBuffer, 20);

    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
    tTaskInit(&tTask3, task3Entry, (void *)0x33333333, 1, &task3Env[1024]);
}


