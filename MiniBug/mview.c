#include "khh.h"

int main()
{
    int err, fd;    
    struct sockaddr_un dest;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    dest.sun_family = AF_UNIX;
    *(dest.sun_path) = 'x';
    strncpy( dest.sun_path, __SOCKET_PATH__, strlen(__SOCKET_PATH__) + 1);
    socklen_t sockaddr_un_len = strlen(dest.sun_path) + sizeof(dest.sun_family);

    fprintf(stderr, "PATH: %s\n", dest.sun_path);

    err = connect(fd, (struct sockaddr*) &dest, sizeof(struct sockaddr_un));

    // do 
    printf("err: %d\n", err);

    char buf[64];

    strncpy(buf, "Test", 4+1);    
    write(fd, buf, 64);

    read(fd, buf, 64);

    strncpy(buf, "Test", 4+1);
    write(fd, buf, 64);

    close(fd);                                                      
    return EXIT_SUCCESS;
}