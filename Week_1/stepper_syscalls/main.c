#include <stdio.h>          // no comment
#include <stdlib.h>         // int64_t, malloc, ...
#include <unistd.h>         // fork, execvp, ...
#include <sys/ptrace.h>     // sys_ptrace
#include <sys/user.h>       // for registers
#include <sys/wait.h>       // for waitpid
#include <sys/reg.h>        // for ORIG_RAX
#include <sys/syscall.h>    // for syscall enums
#include <sched.h>

int main ( int argc, char * argv[] )
{
    pid_t pid;                 
    struct user_regs_struct regs;   // stores register information previously peeked
    int toggle = 0;                 // toggle to remember state (before or after syscall)
    int status;                     // store status info about interrups

    if(!(pid = fork()))             // child
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        sched_yield();
        execvp(argv[1], argv+1);
    } 

    loop:
        
        waitpid(pid, &status, 0);   // wait for interrupt of tracee
        if(WIFEXITED(status))       // tracee exited
        {
            // @exit
            return 0;
        }
        if(toggle)                  // before syscall
        {
            toggle = 0;             // important!            


            // <change this>
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "syscall: RAX = %3lld, RSI = %3lld, RDX = %3lld\n", 
                regs.orig_rax, 
                regs.rsi, 
                regs.rdx);
            
            if(regs.rsi == 42)
            {
                int64_t num = 0;
                ptrace(PTRACE_POKEDATA, pid, regs.rdx, num);
            }
            // </change this>

        }
        else                        // after syscall
        {
            toggle = 1;             // important!


            // <change this>
            ptrace(PTRACE_GETREGS, pid, NULL, &regs);
            fprintf(stderr, "return:  RAX = %3lld, RSI = %3lld, RDX = %3lld\n", 
                regs.rax, 
                regs.rsi, 
                regs.rdx);
            // </change this>     

        }        
        ptrace(PTRACE_SYSCALL, pid, 0,0);   // continue tracee and set breakpoint
        
    
    goto loop;
} 