;mmapstub for jugaad
;compile with nasm 
global _start
%define SYS_mmap	9

;sys/mman.h
%define MAP_PRIVATE 0x02
%define MAP_ANONYMOUS 0x20

%define	PROT_EXEC	0x4
%define	PROT_READ	0x1
%define PROT_WRITE	0x2

%define STACK_SIZE	(4096 * 1024)	;4mb

section	.data

section .text
_start:	
	mov	rdi, 0 
	mov rsi, STACK_SIZE
	mov rdx, PROT_EXEC|PROT_READ|PROT_WRITE
	mov	r10, MAP_PRIVATE|MAP_ANONYMOUS
	;r8 und r9 sind für jugaad nicht relevant ->r8 wäre fd=-1 und r9 ist offset=0
	mov	rax, SYS_mmap
	syscall
	ret