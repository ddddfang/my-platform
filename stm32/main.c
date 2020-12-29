#include "stm32f10x.h"
#include "misc.h"

#include "led.h"
#include "systick.h"
#include "usart.h"

#include "printf.h"

#include "tinyOS.h"


void device_init()  //idleTask in tos.c will call it and create all threads
{
    LED_Init();
    uart_init(115200);
    systick_init(72, 10);

    shell_printf_init();
}


int main(void)
{
    //int val = 0;
    //u8 ch = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //better not change

    tos_init();
    tos_start();

    while(1);
//    while (1) {
//        LED_Set(LED0, Bit_RESET);
//        //ch = uart_getch();
//        //uart_putchar(ch);
//        //delay_ms(1000);
//    }
}


