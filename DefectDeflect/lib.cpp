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
    // todo: DD2 check if breakpoint, if breakpoint, restore data, remove breakpoint, and skip?
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

char* __view_stack__(pid_t pid)
{
    uint64_t rbp = __peek_reg__(pid, RBP);
    uint64_t rsp = __peek_reg__(pid, RSP);    
    uint64_t length = rbp-rsp;
    char* str = (char*) malloc(sizeof(char)*(39*length)+1);
    printf("RBP: %" PRIu64 "\nRSP: %" PRIu64 "\nLength: %" PRIu64 "\n", rbp, rsp, length);
    sleep(1);   // remove these
    assert(str!=NULL);    
    for(uint64_t offset=0; offset<=length; offset++)
    {
        boop((int) offset);
        sleep(1);   // remove these
        uint64_t addr = rsp-(offset+1)*8;
        uint64_t val = __peek_addr__(pid, addr);
        int err = snprintf(str+offset*39, 40, "%#018" PRIx64 ": %#018" PRIx64 "\n" , addr, val);
        assert(err==39);
    }
    printf("HERE COMES THE STACK:\n\n%s\n", str); // remove this
    return str;
}

void __raise_signal__(pid_t pid, int signal)
{
    ptrace(PTRACE_CONT, pid, 0, signal);
    // todo: DD2
    // tracee runs now, add breakpoint right at the spot
}

int __singlestep__(pid_t pid)
{
    uint64_t rip = __peek_reg__(pid, RIP);
    uint64_t data = __peek_addr__(pid, rip);
    uint64_t instr = data & 0xffff;
    fprintf(stderr, "> addr: %#018" PRIx64 "\nnext instruction: %#06" PRIx64 "\n", rip, instr);
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    int status;
    int err = waitpid(pid, &status, 0);
    // todo: DD2
    // check if breakpoint
    // if breakpoint: restore old data
    // and remove breakpoint
    return WIFEXITED(status)?__EXIT__:__RETURN__;

}