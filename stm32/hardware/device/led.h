#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

typedef enum {
    LED0 = 0,
    LED1 = 1
} LED_idx;

void LED_Init(void);
void LED_Set(LED_idx led, BitAction val);
FlagStatus LED_Get(LED_idx led);
void LED_Toggle(LED_idx led);

#endif
