#include "header.hpp"

void __exit__(pid_t pid)
{
    kill(pid, SIGKILL);
}

int __continue__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    int status;
    int err = waitpid(pid, &status, 0);

    __check_for_breakpoint__(pid, breakpoints_ptr);

    return WIFEXITED(status)?__EXIT__:__RETURN__;
}

int __next_syscall__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    int status;

    loop: ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    waitpid(pid, &status, 0);    
    // if a breakpoint was hit, restore and wait for next syscall again
    if(__check_for_breakpoint__(pid, breakpoints_ptr)) goto loop;
    
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
    uint64_t length = (rbp-rsp)/8;
    char* str = (char*) malloc(sizeof(char)*(39*length)+1);
    assert(str!=NULL);    
    for(uint64_t offset=0; offset<length; offset++)
    {
        uint64_t addr = rbp-(offset+1)*8;
        uint64_t val = __peek_addr__(pid, addr);
        int err = snprintf(str+offset*39, 40, "%#018" PRIx64 ": %#018" PRIx64 "\n" , addr, val);
        assert(err==39);
    }
    return str;
}

void __raise_signal__(pid_t pid, int signal)
{
    ptrace(PTRACE_CONT, pid, 0, signal);
}

int __singlestep__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    uint64_t rip = __peek_reg__(pid, RIP);
    uint64_t data = __peek_addr__(pid, rip);
    uint64_t instr = data & 0xffff;
    if(rip == 0xffffffffffffffff) {return __EXIT__;}    
    if(instr != 0x050F) 
    {
        ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    }
    else
    {   
        // calling once for 'enter', then once for 'leave'
        __next_syscall__(pid);
        return __next_syscall__(pid);
    }
    int status;
    int err = waitpid(pid, &status, 0);
    __check_for_breakpoint__(pid, breakpoints_ptr);
    return WIFEXITED(status)?__EXIT__:__RETURN__;

}

void __create_breakpoint__(pid_t pid, uint64_t addr, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    // save old data in breakpoint management
    breakpoints_ptr->insert({addr, __peek_addr__(pid, addr)});
    // write int3, followed by NOPs, into the memory
    __poke_addr__(pid, addr, 0xffffffffffffffccu);
}

int __remove_breakpoint__(pid_t pid, uint64_t addr, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    std::map<uint64_t, uint64_t>::iterator breakpoint = breakpoints_ptr->find(addr);
    if(breakpoint != breakpoints_ptr->end());
    {
        // restore original data
        __poke_addr__(pid, addr, breakpoint->second);
        // remove breakpoint from management
        breakpoints_ptr->erase(breakpoint);
        return 1;
    }
    else
    {
        return 0;
    }
}

void __show_breakpoints__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    std::map<uint64_t, uint64_t>::iterator breakpoint;
    char* result = malloc(sizeof(char)*(breakpoints_ptr->size()*39+1));
    breakpoint = breakpoints_ptr->begin();
    int offset = 0;
    loop:
        if(breakpoint == breakpoints_ptr->end) goto end;
        int err = snprintf( result+offset, 40, "%#018" PRIx64 ": %#018" PRIx64 "\n", 
                            breakpoint->first, breakpoint->second);
        assert(err==39);
        breakpoint++;
        offset += 39;
    goto loop;
    end: return result;
}

void __check_for_breakpoint__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr)
{
    // fetch rip position
    uint64_t rip = __peek_reg__(pid, RIP);
    // if there was a breakpoint, restore old data
    int res = __remove_breakpoint__(pid, --rip, breakpoints_ptr);
    // check if last byte was interrupt, aka if there was a breakpoint
    if(res)
    {
        // decrement the actual RIP
        __poke_reg__(pid, RIP, rip);
        return 1;
    }
    else
    {
        return 0;
    }
}