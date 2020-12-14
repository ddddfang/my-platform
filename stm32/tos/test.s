
.syntax unified

.global test_set
.section .text.test_set

.type test_set, %function
test_set:

    str r1, [r0]
    bx lr

.size test_set, . - test_set



.global switch_ctx
.section .text.switch_ctx

.type switch_ctx, %function
switch_ctx:

    mrs     r0, psp
    cbz     R0, switch_ctx_nosave

    stmfd   r0!, {r4-r11}

    ldr     r1, =currentTask
    ldr     r1, [r1]
    str     r0, [r1]    //store stack pointer to memory[currentTask]

switch_ctx_nosave:

    ldr     r0, =currentTask    //
    ldr     r1, =nextTask       //
    ldr     r2, [r1]            //
    str     r2, [r0]            // currentTask=nextTask
 
    ldr     r0, [r2]
    ldmfd   r0!, {r4-r11}

    msr     psp, r0
    orr     lr, lr, #0x04       //bit or
    bx      lr


.size switch_ctx, . - switch_ctx


