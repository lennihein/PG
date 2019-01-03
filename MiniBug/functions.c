#include "functions.h"

void peek_reg(pid_t pid, int fd)
{
    int err;
    REQUEST req;
    req.control = __RECEIVED__;
    req.value = __ANY__;
    write(fd, &req, __REQ_SIZE__);
    
    err = read(fd, &req, __REQ_SIZE__);
    assert(err == __REQ_SIZE__, "");

    int value = ptrace(PTRACE_PEEKUSER, pid, req.value, NULL);
    
    req.control = __VALUE__;
    req.value = value;
    write(fd, &req, __REQ_SIZE__);

    printf("> Peek Reg\n");
}

void next_syscall(pid_t pid, int fd)
{
    int err;
    REQUEST req;
    req.control = __RECEIVED__;
    req.value = __ANY__;
    write(fd, &req, __REQ_SIZE__);
    
    err = read(fd, &req, __REQ_SIZE__);
    assert(err == __REQ_SIZE__, "");
    int target_call = req.value;
    
    int status, toggle_before;
    toggle_before = 0;
    loop:
    ptrace(PTRACE_SYSCALL, pid, 0,0);
    waitpid(pid, &status, 0);
    if(WIFEXITED(status))       // tracee exited
    {
        // @exit
        return;
    }

    if(toggle_before)
    {
        // @before
        toggle_before = 0;
    }
    else
    {
        // @after
        toggle_before = 1;

        int rax = ptrace(PTRACE_PEEKUSER, pid, ORIG_RAX, NULL);
        if(target_call == -1 || rax == target_call)
        {
            printf("> next_syscall\n");
            req.control = __VALUE__;
            req.value = __ANY__;
            write(fd, &req, __REQ_SIZE__);
            return;
        }
        else
        {
            printf("syscall(%lu) was not wanted syscall\n", 
                    ptrace(PTRACE_PEEKUSER, pid, ORIG_RAX, NULL));
        }

    }   
    goto loop;
}