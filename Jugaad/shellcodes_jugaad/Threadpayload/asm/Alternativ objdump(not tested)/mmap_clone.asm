;mmapstub for jugaad
;compile with nasm -f -elf64 mmap_clone.asm
;ld mmap_clone.o -o mmap_clone
;objdump  -D mmap -M intel>objdump.txt
global _start


%define SYS_mmap	9
%define	SYS_clone	56


;sys/mman.h
%define MAP_PRIVATE 0x02
%define MAP_ANONYMOUS 0x20

%define	PROT_EXEC	0x4
%define	PROT_READ	0x1
%define PROT_WRITE	0x2

%define STACK_SIZE	(4096*1026)	;4mb
;define _SCHED_H_1
;define _USE_GNU 1
;bits/shed.h

%define	CLONE_THREAD 0x00010000
%define CLONE_VM	0x00000100
%define	CLONE_SIGHAND	0x00000800

section	.data


section .text



_start:
	lea rdi,[rel thread_funk]
	;lea rax,rip
	;lea	rdi,[rax+0x47]
	jmp short thread_create1

	


thread_create1:
	push rdi
	jmp short get_stack

thread_create2:
	lea rsi,[rax + STACK_SIZE - 8]
	pop qword [rsi]
	mov rdi, CLONE_THREAD|CLONE_VM|CLONE_SIGHAND
	mov	rax, SYS_clone
	syscall
	int3



get_stack:	
	mov	rdi, 0 
	mov rsi, STACK_SIZE
	mov rdx, PROT_EXEC|PROT_READ|PROT_WRITE
	mov	r10, MAP_PRIVATE|MAP_ANONYMOUS
	;r8 und r9 sind für jugaad nicht relevant ->r8 wäre fd=-1 und r9 ist offset=0
	mov	rax, SYS_mmap
	syscall
	nop
	jmp short thread_create2
	
thread_funk:
		jmp short MainPayload
		str:	db "Hello World", 10
		str_len	equ $ -str
		
MainPayload:		
		mov rax,1
		mov rdi,1
		lea rsi,[rel str]
		mov rdx,str_len
		syscall
		
.exit	mov rdi,0
		mov rax,60
		syscall
