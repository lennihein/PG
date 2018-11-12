#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/reg.h> // for ORIG_RAX
#include <sys/syscall.h>

int main ( int argc, char * argv[] )
{
    pid_t pid;
    struct user_regs_struct regs;
    int status;

    if(!(pid = fork()))
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        close(STDOUT_FILENO);
        execvp(argv[1], argv+1);
    } 

    loop:
        
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) // tracee exited
        {
            // @exit
            return 0;
        }
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);
        fprintf(stderr, "RIP = %llx\n", regs.rip);
        fprintf(stderr, "RAX = %lld,\nRDI = %lld,\nRSI = %lld,\nRDX = %lld\n\n", 
            regs.rax,
            regs.rdi, 
            regs.rsi, 
            regs.rdx);
                 
        ptrace(PTRACE_SINGLESTEP, pid, 0,0);  
    
    goto loop;
} 