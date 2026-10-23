section .asm

extern int21_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

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

no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

%macro interrupt 1
    global int%1
    int%1: 
        ; auto pushed to us by the prcessor upon entry to this interrupt (ip, cs, flags, sp, ss)
        ; interrupt frame start
        pushad 
        ; interrupt frame end
        push esp
        push dword %1
        call interrupt_handler
        add esp, 8
        popad
        iret 
%endmacro 
        
%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

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

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep