section .data

    str: db "Hello World", 10
    str_len equ $ - str         
    ; current adress - str label adress = length of "Hello World\n"
    
section .text
global _start

func:
    push rbp
    mov rbp, rsp
    mov rax, 0x10
    push rax
    mov rax, 0x15
    push rax
    mov rax, 0x1
    push rax
    mov rsp, rbp
    pop rbp
    ret

_start:

    ; sys_write
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str   
    mov RDX, 12     
    syscall

    call func

    ; sys_exit
    mov RAX, 60
    mov RDI, 0          ; error code
    syscall