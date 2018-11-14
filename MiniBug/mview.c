#include "khh.h"

int main()
{
    int err, fd;    
    struct sockaddr_in dest;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    dest.sin_family = AF_INET;
    dest.sin_port = htons(4711);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");
    err = connect(fd, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));

    // do 
    printf("err: %d\n", err);

    char buf[64];
    strncpy(buf, "Test", 4+1); // strlen("Test") + 1 for '\0'    
    write(fd, buf, 64);
    strncpy(buf, "Test", 4+1);
    write(fd, buf, 64);

    close(fd);                                                      
    return EXIT_SUCCESS;
}