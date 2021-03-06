#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"


//void systick_init(u8 SYSCLK);
//void delay_us(u32 nus);
//void delay_ms(u16 nms);

void systick_init(u8 SYSCLK, u8 nms);
u32 systick_getval(void);

#endif
