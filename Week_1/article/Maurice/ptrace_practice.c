#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/reg.h> //for adresses like rax
#include <sys/syscall.h> //syscall to number 
#include <sys/user.h>

//# define ORIG_RAX 59 //sys_execve is syscall 59 in 64 bit
int peekuser();
int peekuserdata();
int getregstest();

int main()
{
    //peekuser(); //prints used syscall
    //peekuserdata(); //prints used syscall and parameter
    getregstest(); //prints used syscall and parameter but with ptrace_getregs and stores in struct user_regs_struct
    return 0;
}

int peekuser()
{
    pid_t child;
    long orig_rax; //var for syscall number
    child = fork();
    if(child == 0) //child
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL); //child being traced
        execl("/bin/ls", "ls", NULL);
    }
    else //parent
    {
        wait(NULL);
        orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL); //syscall number assigned to rax
        printf("The child made a system call %ld\n", orig_rax);
        ptrace(PTRACE_CONT, child, NULL, NULL); //child continues and prints ls 
    }
    return 0;
}
int peekuserdata()
{
    pid_t child;
    long orig_rax, rax;
    long params[3]; //used to store syscall parameter
    int status;
    int insyscall = 0;
    child = fork();
    if(child == 0) 
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL); //child being traced
        execl("/bin/ls", "ls", NULL);
    }
    else 
    {
       while(1) 
       {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
          if(orig_rax == SYS_write) //checks if syscall number in rax is same as for sys_write (in 64 Bit it's 1)
          {
             if(insyscall == 0)
             {
                /* Syscall entry */
                insyscall = 1; //in next iteration "else part" will be executed
                params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RBX, NULL); //necessary parameter are saved in array
                params[1] = ptrace(PTRACE_PEEKUSER,  child, 8 * RCX, NULL);
                params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX,  NULL);
                printf("Write called with %ld, %ld, %ld\n", params[0], params[1], params[2]);  
            }
            else //after child continued(see below "else part"), syscall will be printed
            { /* Syscall exit */
                rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                    printf("Write returned with %ld\n", rax); //prints number of bytes written
                    insyscall = 0; //allows to loop back to "if part"
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL); //continues child
        }
    }
    return 0;
}

int getregstest()
{   pid_t child;
    long orig_rax, rax;
    long params[3];
    int status;
    int insyscall = 0;
    struct user_regs_struct regs; //struct defined in user.h
    child = fork();
    if(child == 0) 
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
    }
    else 
    {
       while(1) 
       {
          wait(&status);
          if(WIFEXITED(status))
              break;
          orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL); //store syscall number
          if(orig_rax == SYS_write) 
          {
              if(insyscall == 0) 
              {
                 /* Syscall entry */
                 insyscall = 1;
                 ptrace(PTRACE_GETREGS, child, NULL, &regs); //stores every parameter in stuct regs
                 printf("Write called with %llu, %llu, %llu\n", regs.rbx, regs.rcx, regs.rdx); //prints necessary parameter
             }
             else 
             { /* Syscall exit */
                 rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                 printf("Write returned with %ld\n", rax);
                 insyscall = 0;
             }
          }
          ptrace(PTRACE_SYSCALL, child,  NULL, NULL);
       }
   }
   return 0;
}
