[BITS 32]

section .asm

global restore_general_purpose_registers
global task_return
global user_registers

; Leave the kernel and start running the task described by the register snapshot.
task_return:
    mov ebp, esp
    mov ebx, [ebp+4] ; access the structure
    push dword [ebx+44] ; data/stack selector
    push dword [ebx+40] ; stack pointer

    ; push the flags
    pushf
    pop eax
    or eax, 0x200
    push eax

    push dword [ebx+32] ; push the code segment
    push dword [ebx+28] ; push the IP to execute

    ; some segments registers
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebx+4]
    call restore_general_purpose_registers
    add esp, 4
    iretd 

; Load general-purpose registers from the task snapshot into the CPU.
restore_general_purpose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

; Point DS, ES, FS, and GS at the user data segment.
user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret