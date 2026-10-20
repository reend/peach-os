section .asm

global tss_load

; Load the TSS selector into the TR register so the CPU knows where the kernel stack is.
tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; tss segment
    ltr ax
    pop ebp
    ret