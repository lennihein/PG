section .data

    str: db "Hello World", 10
    str_len equ $ - str         
    ; current adress - str label adress = length of "Hello World\n"
    
section .text
global _start

_start:

    ; sys_write
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str        
    syscall

    ; sys_write
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str        
    syscall

    ; sys_write
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str        
    syscall

    ; sys_exit
    mov RAX, 60
    mov RDI, 0          ; error code
    syscall