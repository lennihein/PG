#include "mdb.h"

int main()
{
    FILE* log_out = fopen("log.txt", "w");
    int log_fd = fileno(log_out); 
    close(1);           //closing stdout
    dup(log_fd);        //stdout is now written to "log.txt"      
    fclose(log_out);    // closing stream
    close(log_fd);      // closing File Descriptor
                        // only stdout is open, and leads towards log.txt

    fprintf(stderr, "MiniBug service started\n");
    int fd = init_net();

    char buf[__MSG_SIZE__];    
    memset(buf, 0, __MSG_SIZE__);
    read(fd, buf, __MSG_SIZE__);
    fprintf(stdout, "%s\n", buf);
    fprintf(stdout, "writing: ... ");
    strncpy(buf, "Hella", 5);
    write(fd, buf, 10);
    fprintf(stdout, "done\n");
    memset(buf, 0, __MSG_SIZE__);
    read(fd, buf, __MSG_SIZE__);

    fprintf(stderr, "MiniBug shutting down (EXIT_SUCCESS)\n");
    unlink(__SOCKET_PATH__);
    return EXIT_SUCCESS;
}

int init_net()
{
    int fd, socket_fd;
    struct sockaddr_un client_addr, server_addr;
    socklen_t sockaddr_un_len;

    sockaddr_un_len = sizeof(struct sockaddr_un);
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        fprintf(stdout, "Error creating socket\n"); 
        exit(EXIT_FAILURE);
    }
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, __SOCKET_PATH__, strlen(__SOCKET_PATH__) + 1);
    sockaddr_un_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

    if(bind(socket_fd, (struct sockaddr *) &server_addr, sockaddr_un_len) < 0)
    {
       fprintf(stdout, "Error binding socket\n"); 
       exit(EXIT_FAILURE);
    }
    fprintf(stdout, "listening: ..");
    listen(socket_fd, 5);
    fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sockaddr_un_len);  
    fprintf(stdout, ".. Connection established\n");
    return fd;
}