%define SYS_mmap	9
%define STACK_SIZE	(4096*1024)	;4MB
%define DEFAULT_MPROT	0x4 | 0x1 | 0x2
%define DEFAULT_MFLAGS   0x02 | 0x20;wenn flags mit name gesetzt dann erkennt compiler sie nicht-> undefeniert

global _start


_start:
	mov	rdi,0
	mov	rsi,STACK_SIZE
	mov	rdx,DEFAULT_MPROT
	mov r10,DEFAULT_MFLAGS
	mov rax,SYS_mmap
	syscall
	ret
	
;nasm -f elf64 mmapstack.asm
;ld mmapstack.o -o mmapstack
;objdump -D mmapstack -M intel	
; 0000000000400080 <_start>:
  ; 400080:       bf 00 00 00 00          mov    edi,0x0
  ; 400085:       be 00 00 40 00          mov    esi,0x400000
  ; 40008a:       ba 07 00 00 00          mov    edx,0x7
  ; 40008f:       41 ba 22 00 00 00       mov    r10d,0x22
  ; 400095:       b8 09 00 00 00          mov    eax,0x9
  ; 40009a:       0f 05                   syscall
  ; 40009c:       c3                      ret