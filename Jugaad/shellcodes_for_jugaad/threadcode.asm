
%define SYS_clone	56

global start:

_start:
	lea rsi, [rax]		;setzte rsi auf steck ende 
	mov rdi, CLONE_THREAD | CLONE_VM | CLONE_SIGHAND; flags für syscall übergeben
	mov rax, SYS_clone ;syscall laden
	syscall
	ret
	;problem sys_clone erhält keinen pointer für child funktion mehr !