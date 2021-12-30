.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj # TODO: don't want an extern C name here, find out why later

.macro handleException num
.global _ZN16InterruptManager16handleException\num\()Ev
_ZN16InterruptManager16handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro handleInterruptRequest num
.global _ZN16InterruptManager26handleInterruptRequest\num\()Ev
_ZN16InterruptManager26handleInterruptRequest\num\()Ev:
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
    call _ZN16InterruptManager15handleInterruptEhj
    movl %eax, %esp

    # restore processor state after interrupt
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

.global _ZN16InterruptManager22ignoreInterruptRequestEv
_ZN16InterruptManager22ignoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0