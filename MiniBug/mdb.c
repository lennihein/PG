#include "mdb.h"

int main()
{
    fprintf(stderr, "MiniBug service started\n");
    init_log();
    int fd = init_net();

    char buf[__MSG_SIZE__];    
    memset(buf, 0, __MSG_SIZE__);
    read(fd, buf, __MSG_SIZE__);
    printf("%s\n", buf);
    strncpy(buf, "Hella", 5);
    write(fd, buf, 10);
    memset(buf, 0, __MSG_SIZE__);
    read(fd, buf, __MSG_SIZE__);
    printf("%s\n", buf);

    // assert_soft(0, "test");

    fprintf(stderr, "MiniBug shutting down (EXIT_SUCCESS)\n");
    unlink(__SOCKET_PATH__);    // free up file for further use on next startup
    return EXIT_SUCCESS;
}

int init_net()
{
    int fd, socket_fd;
    struct sockaddr_un client_addr, server_addr;
    socklen_t sockaddr_un_len;

    sockaddr_un_len = sizeof(struct sockaddr_un);
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    assert(socket_fd != -1, "Socket Creation Failed");
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, __SOCKET_PATH__, strlen(__SOCKET_PATH__) + 1);
    sockaddr_un_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

    // check if file already exists
    int err = access(__SOCKET_PATH__, F_OK);    // file exists -> err := zero
    assert_soft(err, "Socket File does exist! Unlinking ... ");
    if(!err) unlink(__SOCKET_PATH__);           // unlinking in order to rebind

    err = bind(socket_fd, (struct sockaddr *) &server_addr, sockaddr_un_len);
    assert(err != -1, "Bind failed");

    printf("listening: ..");

    err = listen(socket_fd, 5);
    assert(err != -1, "Listen failed");

    fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sockaddr_un_len);  
    assert(fd != -1, "Accept failed");

    printf(".. Connection established\n");

    return fd;
}

void init_log()
{
    FILE* log_out = fopen("log.txt", "w");
    int log_fd = fileno(log_out); 
    close(1);           //closing stdout
    dup(log_fd);        //stdout is now written to "log.txt"      
    fclose(log_out);    // closing stream
    close(log_fd);      // closing File Descriptor
                        // only stdout is open, and leads towards log.txt
}