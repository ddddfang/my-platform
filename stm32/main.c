#include "stm32f10x.h"
#include "misc.h"

#include "led.h"
#include "systick.h"
#include "usart.h"


int main(void)
{
    u8 ch = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //better not change

    LED_Init();
    systick_init(72);
    uart_init(115200);

    while(ch < 10) {
        LED_Set(LED0, Bit_RESET);
        LED_Set(LED1, Bit_SET);
        delay_ms(1000);
        LED_Set(LED0, Bit_SET);
        LED_Set(LED1, Bit_RESET);
        delay_ms(1000);
        ch++;
    }

    while (1) {
        ch = uart_getch();
        uart_putchar(ch);
        //delay_ms(1000);
    }
}


