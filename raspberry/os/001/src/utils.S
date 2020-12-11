.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.globl put32
put32:
        //store to memory(from register)
        //x0是目标内存地址
	str w1,[x0]
	ret

.globl get32
get32:
        //load to register(from memory), 还有一个用法是 "ldr r, label" 和 "ldr r, =label" (俩伪指令)
        //x0是目标内存地址
	ldr w0,[x0]
	ret

.globl delay
delay:
        //短暂延时,subs: 相减并改变flag寄存器
	subs x0, x0, #1
	bne delay
	ret
