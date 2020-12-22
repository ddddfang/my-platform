
.syntax unified

.global PendSV_Handler
.section .text.PendSV_Handler
.func   PendSV_Handler
.type   PendSV_Handler,%function
.thumb_func
PendSV_Handler:

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

.size   PendSV_Handler, . - PendSV_Handler
.endfunc



