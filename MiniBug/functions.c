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
