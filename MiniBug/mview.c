#include "functions.h"

int init_net(char* exec_path);
void stop(pid_t pid);

int64_t __peek_reg__(pid_t pid, int16_t reg);
void __next_syscall__(pid_t pid, int16_t syscall);

int main()
{
    int fd = init_net("tools/hello_world");
    REQUEST req;

    uint64_t orig_rax, rax, rsp, rbp, rip;

    loop:

        orig_rax = __peek_reg__(fd, ORIG_RAX);
        rax = __peek_reg__(fd, RAX);
        rsp = __peek_reg__(fd, RSP);
        rbp = __peek_reg__(fd, RBP);
        rip = __peek_reg__(fd, RIP);
        printf("ORIG_RAX: %lu\nRAX: %lu\nRSP: %lu\nRBP: %lu\nRIP: %lu\n\n", orig_rax, rax, rsp, rbp, rip);
        __next_syscall__(fd, -1); 

    goto loop;

    stop(fd);
    return EXIT_SUCCESS;
}

/* fixme sockaddr_un_len is never used,
 * fixme do we need to use it instead of sizeof(struct sockaddr_un)?
 */
int init_net(char* exec_path)
{
    int err, fd;
    struct sockaddr_un dest;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    assert(fd != -1, "Socket Creation Failed");
    dest.sun_family = AF_UNIX;
    strncpy(dest.sun_path, __SOCKET_PATH__, strlen(__SOCKET_PATH__) + 1);
    socklen_t sockaddr_un_len = (socklen_t) strlen(dest.sun_path) + sizeof(dest.sun_family);
    err = connect(fd, (struct sockaddr *) &dest, sizeof(struct sockaddr_un));
    assert(err != -1, "Connect failed");

    char buf[__MSG_SIZE__];

    // asking server to trace "touch test.txt"
    strncpy(buf, exec_path, __MSG_SIZE__);
    write(fd, buf, __MSG_SIZE__);

    // awaiting confirmation from server
    read(fd, buf, __MSG_SIZE__);
    assert(!strcmp("successful", buf), "Server creating tracee was unsuccessful");

    return fd;
}

void stop(pid_t fd)
{
    REQUEST req;
    req.control = __CMD__;
    req.value = __EXIT__;
    write(fd, &req, __REQ_SIZE__);
    close(fd);
}

int64_t __peek_reg__(pid_t fd, int16_t reg)
{
    REQUEST req;
    req.control = __CMD__;
    req.value = __PEEK_REG__;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    assert(req.control ==  __RECEIVED__, "");
    req.control = __VALUE__;
    req.value = reg;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    return req.value;
}

void __next_syscall__(pid_t fd, int16_t syscall)
{
    REQUEST req;
    req.control = __CMD__;
    req.value = __NEXT_SYSCALL__;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    assert(req.control ==  __RECEIVED__, "");
    req.control = __VALUE__;
    req.value = syscall;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    if(req.value == __EXIT__)
    {
        printf("Tracee terminated\n");
        exit(0);
    }
    assert(req.control == __VALUE__ && req.value == __ANY__, "");
}