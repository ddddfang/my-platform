#include "stm32f10x.h"
#include "misc.h"

#include "led.h"
#include "systick.h"
#include "usart.h"


extern void test_set(int *a, int v);

int main(void)
{
    int val = 0;
    //u8 ch = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //better not change

    LED_Init();
    uart_init(115200);
    systick_init(72, 250);

    test_set(&val, 1234);

    while (1) {
        //ch = uart_getch();
        //uart_putchar(ch);
        //delay_ms(1000);
    }
}


