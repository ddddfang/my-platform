/*************************************** Copyright (c)******************************************************
** File name            :   app.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   应用代码
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
#include "tinyOS.h"
#include "led.h"
#include "usart.h"

// 任务1和任务2的任务结构，以及用于堆栈空间
tTask tTask1;
tTask tTask2;
tTaskStack task1Env[1024];
tTaskStack task2Env[1024];

void task1Entry (void * param) 
{
    for (;;)
    {
        LED_Set(LED0, Bit_SET);
        LED_Set(LED1, Bit_RESET);
        uart_putchar('A');
        tTaskDelay(5);
    }
}

void task2Entry (void * param) 
{
    for (;;) 
    {
        LED_Set(LED0, Bit_RESET);
        LED_Set(LED1, Bit_SET);
        uart_putchar('B');
        tTaskDelay(10);
    }
}


void tInitApp (void)
{
    // 初始化任务1和任务2结构，传递运行的起始地址，想要给任意参数，以及运行堆栈空间
    tTaskInit(&tTask1, task1Entry, (void *)0x11111111, 0, &task1Env[1024]);
    tTaskInit(&tTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
}


