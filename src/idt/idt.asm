section .asm

extern int21_handler
extern no_interrupt_handler
extern isr80h_handler

global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    
    pop ebp
    ret

int21h:
    pushad
    call int21_handler
    popad
    iret

no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

isr80h_wrapper:
    ; auto pushed to us by the prcessor upon entry to this interrupt (ip, cs, flags, sp, ss)
    ; push general purpose registers
    ; interrupt frame start
    pushad 
    ; interrupt frame end

    push esp ; point to interrupt frame
    push eax ; holds our command for isr80h_handler
    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8

    ; regtore general purpose registers for user land
    popad
    mov eax, [tmp_res]
    iretd

section .data
; return result from isr80h_handler
tmp_res: dd 0