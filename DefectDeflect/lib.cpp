#include "header.hpp"

void __exit__(pid_t pid)
{
    kill(pid, SIGKILL);
}

int __continue__(pid_t pid)
{
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    int status;
    int err = waitpid(pid, &status, 0);

    // todo: DD2
    // check if breakpoint
    // if breakpoint: restore old data
    // and remove breakpoint

    return WIFEXITED(status)?__EXIT__:__RETURN__;
}

int __next_syscall__(pid_t pid)
{
    int status;
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    waitpid(pid, &status, 0);
    // todo: DD2 check if breakpoint, if breakpoint, skip
    if(WIFEXITED(status))
    {
        return __EXIT__;
    }
    uint64_t rax = __peek_reg__(pid, RAX);
    return 18446744073709551578u!=rax?1:0;
}

uint64_t __peek_reg__(pid_t pid, uint64_t reg)
{
    return ptrace(PTRACE_PEEKUSER, pid, reg, NULL);
}

void __poke_reg__(pid_t pid, uint64_t reg, uint64_t data)
{
    ptrace(PTRACE_POKEUSER, pid, reg, data);
}

uint64_t __peek_addr__(pid_t pid, uint64_t addr)
{
    return ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
}

void __poke_addr__(pid_t pid, uint64_t addr, uint64_t data)
{
    ptrace(PTRACE_POKETEXT, pid, addr, data);
}