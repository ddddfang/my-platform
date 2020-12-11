ARMGNU ?= aarch64-linux-gnu

# -Wall, 显示所有警告信息, Warning all
# -nostdlib, 不使用C标准库(因为几乎所有的c api都会调用到os)
# -nostartfiles, 不使用标准启动文件,启动文件负责初始化堆栈指针,初始化static数据并跳转到 main entry point
# -ffreestanding, 告诉编译器没有标准库存在,程序入口也不一定在main
# -mgeneral-regs-only, 只使用通用寄存器 (ARM 处理器还有 NEON registers)

COPS = -Wall -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only
ASMOPS = -Iinclude 

BUILD_DIR = build
SRC_DIR = src

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)


#下面的 -MMD 就是告诉gcc生成.o对应的依赖文件
#包含所有的.d文件这样每次make 就知道哪些文件发生了改动

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)



all : kernel8.img

clean :
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	# $(@D) 直接指代了目标文件的目录
	mkdir -p $(@D)

	# -MMD 指示gcc为每一个.o文件产生对应的依赖文件 *.d,这样.h文件的改动也会触发gcc的编译 
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc $(ASMOPS) -MMD -c $< -o $@


kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	# 指定使用 linker.ld作为链接脚本, 产出的文件为elf格式 (designed to be executed by an os)
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)

	# objcopy的过程就是去除elf中对我们无用的 elf header,只抽取其中的代码段数据段
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
