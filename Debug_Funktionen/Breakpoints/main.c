#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <stdint.h>

# define R15	    8*00
# define R14	    8*01
# define R13	    8*02
# define R12	    8*03
# define RBP	    8*04
# define RBX	    8*05
# define R11	    8*06
# define R10	    8*07
# define R9	        8*08
# define R8	        8*09
# define RAX	    8*10
# define RCX	    8*11
# define RDX	    8*12
# define RSI	    8*13
# define RDI	    8*14
# define ORIG_RAX   8*15
# define RIP	    8*16
# define CS	        8*17
# define EFLAGS	    8*18
# define RSP	    8*19
# define SS	        8*20
# define FS_BASE    8*21
# define GS_BASE    8*22
# define DS	        8*23
# define ES	        8*24
# define FS	        8*25
# define GS	        8*26

const int long_size=sizeof(long);

void print_bytes(void *ptr, int size)
{
    unsigned char *p = ptr;
    int i;
    for (i=0; i<size; i++) {
        printf("0x%02hhX ", p[i]);
    }
    printf("\n");
}

int main(int argc, char * argv[])
{
	uint64_t rip, rax, rsp, rbp, content;
	pid_t child;
	struct user_regs_struct regs;
	int status;
	child = fork();
	pid_t pid = child;
	if(child == -1) {exit(1);}
	if(child ==  0)
	{
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL))
		{
			perror("ptrace");
			return 1;
		}
		execvp(argv[1], argv+1);
		
	}
	else{
		wait(NULL);

		uint64_t* code = malloc(8);
		memset(code, 0x90, 8);
		*((uint8_t*) code) = 0xcc;
		int shellcode_len = 8;
		uint64_t* backup = malloc(shellcode_len);

		// print rip
		rip = ptrace(PTRACE_PEEKUSER, pid, RIP, 0);		
		fprintf(stderr, "rip= %lu\n", rip);

		// save rip as 'addr'
		uint64_t addr = rip;

		fprintf(stderr, "\ndata bereich holen\n\n");
		for(int i = 0; i < shellcode_len; i+=8)
		{
			backup[i] = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		}

		fprintf(stderr, "\ncode\n");
		print_bytes(code, 8);
		fprintf(stderr, "\nbackup\n");
		print_bytes(backup, 8);

		fprintf(stderr, "\ncode laden\n");
		for(int i = 0; i < shellcode_len; i+=8)
		{
			ptrace(PTRACE_POKEDATA, pid, addr + i, code[i]);
		}

		fprintf(stderr, "\nbytes at addr are:\n");
		content = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		print_bytes(&content, 8);
				
		fprintf(stderr, "\ncont\n");		
		ptrace(PTRACE_CONT, child, NULL, NULL);
		
		wait(NULL);
		
		// breakpoint 
		getchar();
		
		fprintf(stderr, "\nbytes at addr are:\n");
		content = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		print_bytes(&content, 8);

		//trapfunktion mit backup wieder überschreiben
		for(int i = 0; i < shellcode_len; i+=8)
		{
			ptrace(PTRACE_POKEDATA, pid, addr + i, backup[i]);
		}

		fprintf(stderr, "\nnow backup loaded bytes at addr are:\n");
		content = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		print_bytes(&content, 8);
		
		rip = ptrace(PTRACE_PEEKUSER, pid, RIP, 0);		
		fprintf(stderr, "\nchange rip from %lu to %lu\n", rip, addr);
		ptrace(PTRACE_POKEUSER, pid, RIP, addr);

		rip = ptrace(PTRACE_PEEKUSER, pid, RIP, 0);		
		fprintf(stderr, "rip = %lu\n", rip);

		fprintf(stderr, "\nloaded back bytes:\n");
		content = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		print_bytes(&content, 8);
		
		fprintf(stderr, "\noutput: ");

		ptrace(PTRACE_CONT, pid, NULL, NULL);
		
		wait(NULL);
	}
}
