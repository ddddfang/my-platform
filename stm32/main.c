#include "stm32f10x.h"
#include "misc.h"

#include "led.h"
#include "systick.h"
#include "usart.h"


extern void test_set(int *a, int v);

int main(void)
{
    int val = 0;
    u8 ch = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //better not change

    LED_Init();
    systick_init(72);
    uart_init(115200);

    test_set(&val, 1234);
    if (val == 1234) {
        ch = 0;
    } else {
        ch = 10;
    }

    while(ch < 10) {
        LED_Toggle(LED0);
        if ((ch % 2) == 0) {
            LED_Set(LED1, Bit_SET);
        } else {
            LED_Set(LED1, Bit_RESET);
        }
        delay_ms(1000);
        ch++;
    }

    while (1) {
        ch = uart_getch();
        uart_putchar(ch);
        //delay_ms(1000);
    }
}


