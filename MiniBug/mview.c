#include "functions.h"

int init_net(char* exec_path);
void stop(pid_t pid);

int main()
{
    int fd = init_net("tools/hello_world");
    REQUEST req;

    // next_syscall(SYS_WRITE)
    req.control = __CMD__;
    req.value = __NEXT_SYSCALL__;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    assert(req.control ==  __RECEIVED__, "");
    req.control = __VALUE__;
    req.value = SYS_WRITE;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    assert(req.control == __VALUE__ && req.value == __ANY__, "");

    // peek_reg(RAX)
    req.control = __CMD__;
    req.value = __PEEK_REG__;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    assert(req.control ==  __RECEIVED__, "");
    req.control = __VALUE__;
    req.value = RAX;
    write(fd, &req, __REQ_SIZE__);
    read(fd, &req, __REQ_SIZE__);
    printf("Number of printed characters %lu\n", req.value);

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