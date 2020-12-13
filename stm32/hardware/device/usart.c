#include "usart.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

void uart_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  //enable USART 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //enable GPIO 时钟

    //初始化GPIO
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;  //GPIOA.9是USART1的TX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; //设置其模式为推挽复用输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz; //速度为10MHz
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;  //GPIOA.10是USART1的RX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //设置其模式为浮空输入
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;  //速度为10MHz
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //初始化串口
    USART_DeInit(USART1);
    USART_InitStruct.USART_BaudRate = bound; //设置波特率
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //不使用这个
    USART_InitStruct.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);  //设置模式为即接受又发送
    USART_InitStruct.USART_Parity = USART_Parity_No;  //无奇偶校验位，也就是无第九位
    USART_InitStruct.USART_StopBits = USART_StopBits_1;  //1位停止位
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; //8位数据位
    USART_Init(USART1, &USART_InitStruct);

    //
    NVIC_InitTypeDef NVIC_InitStruct;
    //初始化NVIC(设置抢占优先级和响应优先级)
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;  //选择IRQ为USART1
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;    //使能IRQ通道
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;  //设置抢占优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2; //设置响应优先级
    NVIC_Init(&NVIC_InitStruct);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //设置接受到数据触发中断

    //使能串口
    USART_Cmd(USART1, ENABLE);
}

int uart_putchar(char ch)
{
    while((USART1->SR & (1<<6)) == 0);
    USART_SendData(USART1, (uint16_t)ch);
    return 0;
}

#include "queue.h"
int uart_getch(void)
{
    int cr;
    while(queue_len(&usart_queue) == 0);
    cr = dequeue(&usart_queue);
    return cr;
}


