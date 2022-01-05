.set IRQ_BASE, 0x20

.section .text

.extern _ZN5NBSOS8Hardware16InterruptManager15handleInterruptEhj

.macro handleException num
.global _ZN5NBSOS8Hardware16InterruptManager15handleException\num\()Ev
_ZN5NBSOS8Hardware16InterruptManager15handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro handleInterruptRequest num
.global _ZN5NBSOS8Hardware16InterruptManager26handleInterruptRequest\num\()Ev
_ZN5NBSOS8Hardware16InterruptManager26handleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

handleInterruptRequest 0x00
handleInterruptRequest 0x01
handleInterruptRequest 0x0C


int_bottom:
    # backup processor state from before interrupt
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN5NBSOS8Hardware16InterruptManager15handleInterruptEhj
    movl %eax, %esp

    # restore processor state after interrupt
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

.global _ZN5NBSOS8Hardware16InterruptManager22ignoreInterruptRequestEv
_ZN5NBSOS8Hardware16InterruptManager22ignoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0