#include "mdb.h"

int main()
{
    fprintf(stderr, "MiniBug service started\n");
    init_log();
    int fd = init_net();
    int err;

    char buf[__MSG_SIZE__];    
    memset(buf, 0, __MSG_SIZE__);
    err = read(fd, buf, __MSG_SIZE__);
    assert(err == __MSG_SIZE__, "read unsuccessful");
    printf("%s\n", buf);
    pid_t pid = init_tracee(buf);
    // tracee now created and traced

    strncpy(buf, "successful", __MSG_SIZE__);
    write(fd, buf, __MSG_SIZE__);
    // client notified

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

pid_t init_tracee(char* str)
{
    //convert string to tokens
    int counter = 1;
    for(int i = 0; str[i] != 0x0; i++)
    {
        if(str[i] == 32) counter++;
    }
    char** tokens = malloc(sizeof(void*) * counter);
    counter = 0;
    tokens[0] = str+counter;
    for(int i = 0; str[i] != 0x0; i++)
    {
        if(str[i] == 32) 
        {
            tokens[++counter] = str+i+1;
            str[i] = 0x0;
        }
    }

    fprintf(stderr, "%s", tokens[0]);

    // create tracee
    pid_t pid;
    int status, err;
    pid = fork();
    assert(pid != -1, "fork failed");

    if(!(pid)) // child
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        // close(STDOUT_FILENO);
        err = execvp(tokens[0], tokens);
        exit(EXIT_FAILURE);
    } 

    // wait for tracee
    waitpid(pid, &status, 0);
    assert(!WIFEXITED(status), "execvp() failed");
    printf("Tracee created, traced, and interrupted\n");
    return pid;
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
    printf("Log output redirected\n");
}