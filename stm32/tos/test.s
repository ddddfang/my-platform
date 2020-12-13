
.global test_set


.section .text.test_set

.type test_set, %function
test_set:

    str r1, [r0]
    bx lr

.size test_set, . - test_set
