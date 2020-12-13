#include "led.h"
#include "stm32f10x_rcc.h"

typedef struct {
    GPIO_TypeDef *base;
    uint16_t pinidx;
} LED_cfg;

static LED_cfg leds[2] = {
    {GPIOB, GPIO_Pin_5},
    {GPIOE, GPIO_Pin_5}
};

void LED_Init(void)
{
    GPIO_InitTypeDef ioLed0;    //PB5
    GPIO_InitTypeDef ioLed1;    //PE5

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    ioLed0.GPIO_Pin = GPIO_Pin_5;
    ioLed0.GPIO_Mode = GPIO_Mode_Out_PP;
    ioLed0.GPIO_Speed = GPIO_Speed_50MHz;
    ioLed1.GPIO_Pin = GPIO_Pin_5;
    ioLed1.GPIO_Mode = GPIO_Mode_Out_PP;
    ioLed1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &ioLed0);
    GPIO_Init(GPIOE, &ioLed1);

    LED_Set(LED0, Bit_SET);
    LED_Set(LED1, Bit_SET);
}

void LED_Set(LED_idx led, BitAction val)
{
    switch (val) {
        case Bit_SET:
            GPIO_SetBits(leds[led].base, leds[led].pinidx);
            break;
        case Bit_RESET:
            GPIO_ResetBits(leds[led].base, leds[led].pinidx);
            break;
    }
}

FlagStatus LED_Get(LED_idx led)
{
    return (FlagStatus)GPIO_ReadOutputDataBit(leds[led].base, leds[led].pinidx);
}

void LED_Toggle(LED_idx led)
{
    if (LED_Get(led) == SET) {
        LED_Set(led, Bit_RESET);
    } else {
        LED_Set(led, Bit_SET);
    }
    //leds[led].base->ODR ^= (1 << leds[led].pinidx);
}
