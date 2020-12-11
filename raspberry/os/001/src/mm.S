.globl memzero
memzero:
        // str: store from register (to memory), after done, x0 += 8
        // 64bit arm 32个通用寄存器: x0-x30 + xzr(专门存放0的寄存器)
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret
