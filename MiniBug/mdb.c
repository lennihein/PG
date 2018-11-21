#include "khh.h"

int main()
{
    fprintf(stderr, "MiniBug service started\n");

    int fd, new_sock;
    struct sockaddr_in client_addr, server_addr;
    socklen_t sockaddr_in_len;

    sockaddr_in_len = sizeof(struct sockaddr_in);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(__PORT__);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd, (struct sockaddr *) &server_addr, sockaddr_in_len);
    listen(fd, 5);
    new_sock = accept(fd, (struct sockaddr *) &client_addr, &sockaddr_in_len);
    fprintf(stderr, "Received connection from %s\n", inet_ntoa(client_addr.sin_addr));
    if(*(uint32_t*) &client_addr.sin_addr != 16777343) // 0x7F . 0x0 . 0x0 . 0x1
    {
        fprintf(stderr, "Not LOOPBACK\nAbort\n");
        return EXIT_FAILURE;
    }

    // do

    char buf[2048];

    memset(buf, 0, 2048);
    read(new_sock, buf, 2048);
    fprintf(stderr, "%s\n", buf);

    memset(buf, 0, 2048);
    read(new_sock, buf, 2048);
    fprintf(stderr, "%s\n", buf);

    memset(buf, 0, 2048);
    read(new_sock, buf, 2048);
    fprintf(stderr, "%s\n", buf);

    fprintf(stderr, "MiniBug shutting down (EXIT_SUCCESS)\n");
    return EXIT_SUCCESS;
}