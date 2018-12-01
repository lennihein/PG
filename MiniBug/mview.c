#include "khh.h"

int init_net();

int main()
{
    int fd = init_net();

    char buf[__MSG_SIZE__];

    // asking server to trace "touch test.txt"
    strncpy(buf, "touch test.txt", __MSG_SIZE__);    
    write(fd, buf, __MSG_SIZE__);

    // awaiting confirmation from server
    read(fd, buf, __MSG_SIZE__);
    assert(!strcmp("successful", buf), "Server creating tracee was unsuccessful");

    close(fd);                                                      
    return EXIT_SUCCESS;
}

int init_net()
{
    int err, fd;    
    struct sockaddr_un dest;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    assert(fd!=-1, "Socket Creation Failed");
    dest.sun_family = AF_UNIX;
    strncpy( dest.sun_path, __SOCKET_PATH__, strlen(__SOCKET_PATH__) + 1);
    socklen_t sockaddr_un_len = strlen(dest.sun_path) + sizeof(dest.sun_family);
    err = connect(fd, (struct sockaddr*) &dest, sizeof(struct sockaddr_un));
    assert(err != -1, "Connect failed");

    return fd;
}