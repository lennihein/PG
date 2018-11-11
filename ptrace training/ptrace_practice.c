#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/reg.h>
#include <sys/syscall.h>

//# define ORIG_RAX 59 //sys_execve is syscall 59 in 64 bit
int peekuser();
int nexttest();

int main()
{
    //peekuser();
    nexttest();
    return 0;
}

int peekuser()
{
    pid_t child;
    long orig_rax;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
        wait(NULL);
        orig_rax = ptrace(PTRACE_PEEKUSER,
                          child, 8 * ORIG_RAX,
                          NULL);
        printf("The child made a "
               "system call %ld\n", orig_rax);
        ptrace(PTRACE_CONT, child, NULL, NULL);
    }
    return 0;
}
int nexttest()
{
    pid_t child;
    long orig_rax, rax;
    long params[3];
    int status;
    int insyscall = 0;
    child = fork();
    if(child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else {
       while(1) {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_rax = ptrace(PTRACE_PEEKUSER,
                     child, 8 * ORIG_RAX, NULL);
            /*if(orig_rax<0)
            {
                printf("Syscall is %li and below 0",orig_rax);
                return 1;
            }*/
          if(orig_rax == SYS_write) {
             if(insyscall == 0) {
                /* Syscall entry */
                insyscall = 1;
                params[0] = ptrace(PTRACE_PEEKUSER,
                                   child, 8 * RBX,
                                   NULL);
                params[1] = ptrace(PTRACE_PEEKUSER,
                                   child, 8 * RCX,
                                   NULL);
                params[2] = ptrace(PTRACE_PEEKUSER,
                                   child, 8 * RDX,
                                   NULL);
                printf("Write called with "
                       "%ld, %ld, %ld\n",
                       params[0], params[1],
                       params[2]);  
                }
          else { /* Syscall exit */
                rax = ptrace(PTRACE_PEEKUSER,
                             child, 8 * RAX, NULL);
                    printf("Write returned "
                           "with %ld\n", rax);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL,
                   child, NULL, NULL);
        }
    }
    return 0;
}