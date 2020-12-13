#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

void uart_init(u32 bound);
int uart_putchar(char ch);
int uart_getch(void);

#endif

