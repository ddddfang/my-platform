
#startup
OBJS += hardware/startup/cSTARTUP_STM32F103xE.o     # c version
#OBJS += hardware/startup/sSTARTUP_STM32F103xE.o     # asm version

#lib files
OBJS += hardware/lib/src/core/core_cm3.o
OBJS += hardware/lib/src/core/system_stm32f10x.o
OBJS += hardware/lib/src/peripherals/misc.o
OBJS += hardware/lib/src/peripherals/stm32f10x_dac.o
OBJS += hardware/lib/src/peripherals/stm32f10x_gpio.o
OBJS += hardware/lib/src/peripherals/stm32f10x_sdio.o
OBJS += hardware/lib/src/peripherals/stm32f10x_adc.o
OBJS += hardware/lib/src/peripherals/stm32f10x_dbgmcu.o
OBJS += hardware/lib/src/peripherals/stm32f10x_i2c.o
OBJS += hardware/lib/src/peripherals/stm32f10x_spi.o
OBJS += hardware/lib/src/peripherals/stm32f10x_bkp.o
OBJS += hardware/lib/src/peripherals/stm32f10x_dma.o
OBJS += hardware/lib/src/peripherals/stm32f10x_iwdg.o
OBJS += hardware/lib/src/peripherals/stm32f10x_tim.o
OBJS += hardware/lib/src/peripherals/stm32f10x_can.o
OBJS += hardware/lib/src/peripherals/stm32f10x_exti.o
OBJS += hardware/lib/src/peripherals/stm32f10x_pwr.o
OBJS += hardware/lib/src/peripherals/stm32f10x_usart.o
OBJS += hardware/lib/src/peripherals/stm32f10x_cec.o
OBJS += hardware/lib/src/peripherals/stm32f10x_flash.o
OBJS += hardware/lib/src/peripherals/stm32f10x_rcc.o
OBJS += hardware/lib/src/peripherals/stm32f10x_wwdg.o
OBJS += hardware/lib/src/peripherals/stm32f10x_crc.o
OBJS += hardware/lib/src/peripherals/stm32f10x_fsmc.o
OBJS += hardware/lib/src/peripherals/stm32f10x_rtc.o

#user files
OBJS += main.o
OBJS += stm32f10x_it.o

OBJS += common/queue.o

OBJS += hardware/device/led.o
OBJS += hardware/device/systick.o
OBJS += hardware/device/usart.o

OBJS += common/tLibBitmap.o
OBJS += common/tLibList.o
OBJS += system/printf.o

#OBJS += $(patsubst %.c,%.o ,$(wildcard ./tos/*.c))
#OBJS += $(patsubst %.c,%.o ,$(wildcard ./tos/*.s))
OBJS += tos/switch.o
OBJS += tos/tTask.o
OBJS += tos/tTime.o
OBJS += tos/tEvent.o
OBJS += tos/tSem.o
OBJS += tos/tMBox.o
OBJS += tos/tos.o
OBJS += tos/app.o


PROJ_NAME = main


CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
AS = arm-none-eabi-as
OBJCOPY = arm-none-eabi-objcopy


#链接脚本使用 ./linker103xe.ld
LDFLAGS = -Tlinker103xe.ld

#about --specs=nosys.specs, see:
# https://stackoverflow.com/questions/19419782/exit-c-text0x18-undefined-reference-to-exit-when-using-arm-none-eabi-gcc
LDFLAGS += --specs=nosys.specs
LDFLAGS += -ffreestanding
# 下面这一行注释掉汇编版本的启动文件里才可以调用 __libc_init_array,但是得到的hex没法执行?
LDFLAGS += -nostdlib

#-flto: Link Time Optimization(尚不清楚优化在哪里)
#-Wl,--gc-sections 告诉编译器去掉没有用到的section
LDFLAGS += -flto -Wl,--gc-sections -Wl,-Map=$(PROJ_NAME).map


CFLAGS = -g -O1 -DCALL_ARM_SYSTEM_INIT -DSTM32F10X_HD
CFLAGS += -Wall

CFLAGS += -mcpu=cortex-m3 -march=armv7-m -mthumb
CFLAGS += -mlittle-endian
CFLAGS += -masm-syntax-unified

CFLAGS += -nostdlib
CFLAGS += -fno-common
CFLAGS += -ffreestanding
CFLAGS += -fno-exceptions
CFLAGS += -fno-unwind-tables

# 链接是以section为单位的,这里指示编译器为每个函数和符号生成同名 section
# 这样配合 -Wl,--gc-sections 可以去掉没有用到的section,生成比较小的elf
# https://www.cnblogs.com/dylancao/archive/2004/01/13/9481160.html
CFLAGS += -ffunction-sections -fdata-sections

CFLAGS += -I.
CFLAGS += -Icommon
CFLAGS += -Ihardware/lib/inc
CFLAGS += -Ihardware/lib/inc/core
CFLAGS += -Ihardware/lib/inc/peripherals
CFLAGS += -Ihardware/device
CFLAGS += -Itos
CFLAGS += -Isystem

###################################################

.PHONY: proj
all: $(PROJ_NAME).elf

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^
%.o:%.s
	$(CC) $(CFLAGS) -c -o $@ $^

$(PROJ_NAME).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f $(OBJS)
	rm -f $(PROJ_NAME).map
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
