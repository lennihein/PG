#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/reg.h> // for ORIG_RAX
#include <sys/syscall.h>
#include <sched.h>

int main ( int argc, char * argv[] )
{
    int x = 10;

    pid_t pid;
    struct user_regs_struct regs;
    long long syscall;
    int toggle = 1;
    int status;

    if(!(pid = fork()))
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], argv+1);
    } 

    loop:
        
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) // tracee exited
        {
            // @exit
            return 0;
        }
        if(toggle) //after syscall
        {
            toggle = 0;
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "@a syscall: %lld (%lld), RSI = %lld, RDX = %lld\n"
                , regs.orig_rax, regs.rax, regs.rsi, regs.rdx);
        }
        else // before syscall
        {
            toggle = 1;   

            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "@b syscall: %lld (%lld), RSI = %lld, RDX = %lld\n"
                , regs.orig_rax, regs.rax, regs.rsi, regs.rdx);
            
            if(regs.rsi == 42) // change
            {
                long long num = 0;
                ptrace(PTRACE_POKEDATA, pid, regs.rdx, num);
            }

        }        
        ptrace(PTRACE_SYSCALL, pid, 0,0);
        
    
    goto loop;
} 