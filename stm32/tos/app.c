
#include "tinyOS.h"
#include "led.h"
#include "usart.h"

// 任务1和任务2的任务结构，以及用于堆栈空间
static tTask tTask1;
static tTask tTask2;
static tTaskStack task1Env[1024];
static tTaskStack task2Env[1024];

void task1Entry (void * param) 
{
    while(!tTaskIsRequestedDelete()) {
        uart_putchar('A');
        tTaskDelay(100); //100*10ms
//        LED_Set(LED1, Bit_SET);
//        tTaskDelay(100); //
//        LED_Set(LED1, Bit_RESET);
    }
}

void task2Entry (void * param)
{
    while(!tTaskIsRequestedDelete()) {
        uart_putchar('B');
        tTaskDelay(47);
//        LED_Set(LED0, Bit_SET);
//        tTaskDelay(47);
//        LED_Set(LED0, Bit_RESET);
    }
}


void tInitApp (void)   //idleTask in tos.c will call it and init devices
{
    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
}


