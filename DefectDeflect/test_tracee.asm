section .data

    str: db "Hello World", 10
    str_len equ $ - str         
    ; current adress - str label adress = length of "Hello World\n"
    
section .text
global _start

_start:

    ; sys_write
    mov RAX, 1
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str   
    mov RDX, 12     
    syscall
    mov RAX, 1

    ; sys_exit
    mov RAX, 60
    mov RDI, 0          ; error code
    syscall