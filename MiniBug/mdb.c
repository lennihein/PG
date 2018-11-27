#include "mdb.h"

typedef struct net_info
{    
    int fd, new_sock;
    struct sockaddr_in client_addr, server_addr;
    socklen_t sockaddr_in_len;

} net_info;

int main()
{
    fprintf(stderr, "MiniBug service started\n");
    int fd = init_net();

    char buf[2048];
    
    memset(buf, 0, 2048);
    read(fd, buf, 2048);
    fprintf(stderr, "%s\n", buf);

    fprintf(stderr, "writing: ... ");
    strncpy(buf, "Hella", 5);
    write(fd, buf, 10);
    fprintf(stderr, "done\n");

    memset(buf, 0, 2048);
    read(fd, buf, 2048);
    fprintf(stderr, "%s\n", buf);

    fprintf(stderr, "MiniBug shutting down (EXIT_SUCCESS)\n");
    return EXIT_SUCCESS;
}

int init_net()
{
    int fd, socket_fd;
    struct sockaddr_in client_addr, server_addr;
    socklen_t sockaddr_in_len;

    sockaddr_in_len = sizeof(struct sockaddr_in);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(__PORT__);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(socket_fd, (struct sockaddr *) &server_addr, sockaddr_in_len);
    listen(socket_fd, 5);
    fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sockaddr_in_len);
    fprintf(stderr, "Received connection from %s\n", inet_ntoa(client_addr.sin_addr));
    if(*(uint32_t*) &client_addr.sin_addr != 16777343) // 0x7F . 0x0 . 0x0 . 0x1
    {
        fprintf(stderr, "Not LOOPBACK\nAbort\n");
        return EXIT_FAILURE;
    }    
    fprintf(stderr, "Connection established\n");
    return fd;
}