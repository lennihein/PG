SYS_WRITE equ 1
SYS_EXIT equ 60
STD_OUTPUT equ 1
 
section .text
global _start
 
_start:
jmp short MainCode
	msg: db `Shellcode: "Hello world!"\n`
	msglen equ $-msg
 
MainCode:
	mov rax, SYS_WRITE
	mov rdi, STD_OUTPUT
	mov rsi, msg
	mov rdx, msglen
	syscall
 
	mov rax, SYS_EXIT
	mov rdi, 0
	syscall