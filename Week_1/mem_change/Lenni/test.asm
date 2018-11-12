section .text
global _start

_start:

    mov rax, 1
    push rax
    mov rsi, 42
    mov RAX, 24
    MOV RDX, rsp
    syscall ; 24

    mov rax, 24
    mov rsi, 13
    pop rdx
    syscall ; 24

    ; sys_exit
    mov RAX, 60
    mov RDI, 0    
    syscall ; 60