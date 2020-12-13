#include "systick.h"
#include "misc.h"

static u8  fac_us = 0;
static u16 fac_ms = 0;

void systick_init(u8 SYSCLK)  //SYSCLK, MHz
{
    //SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;   //AHB/8 as the clock source
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//AHB/8 as the clock source
    fac_us = SYSCLK / 8;
    fac_ms = (u16)fac_us * 1000;
}

void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

//nms <= 1864
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms*fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}
