#include "header.hpp"

void __exit__(pid_t pid)
{
    kill(pid, SIGKILL);
}

int __continue__(pid_t pid)
{
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    waitpid(pid, 0, 0);

    // check if breakpoint
    // if breakpoint: restore old data
    // and remove breakpoint

    // check if tracee exited

    // return some code
    return 42;
}