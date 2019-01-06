#include "mdb.h"

/*  could be useful
*
*   void checked_read(int fd, char* buf) { assert((int) read(fd, buf, __MSG_SIZE__)) == __MSG_SIZE__, "read unsuccessful"); }
*
*   or a macro?
*
*   #define checked_read(fd,buf) err=(int)read(fd,buf,__MSG_SIZE__);assert(err==__MSG_SIZE__,"read\ unsuccessful");
*/

int main()
{
    // initialise
    fprintf(stderr, "MiniBug service started\n");
    init_log();
    int fd = init_net();
    int err;
    char buf[__MSG_SIZE__];

    // awaiting target from client
    err = (int) read(fd, buf, __MSG_SIZE__);
    assert(err == __MSG_SIZE__, "read unsuccessful");
    printf("%s\n", buf);

    // creating and tracing tracee
    pid_t pid = init_tracee(buf);

    // notifying client
    strncpy(buf, "successful", __MSG_SIZE__);
    write(fd, buf, __MSG_SIZE__);

    // debug routine
    debug_loop(pid, fd);

    // destroy
    fprintf(stderr, "MiniBug shutting down (EXIT_SUCCESS)\n");
    unlink(__SOCKET_PATH__);    // free up file for further use on next startup
    return EXIT_SUCCESS;
}

void debug_loop(pid_t pid, int fd)
{
    int err;
    REQUEST req;    

    loop: printf("> debug loop .... \n");
    err = (int) read(fd, &req, __REQ_SIZE__);
    assert(err == __REQ_SIZE__, "read unsuccessful");
    assert(req.control == __CMD__, "did not receive a command");

    switch(req.value) 
    {
        case __EXIT__:                                  goto exit;
        case __PEEK_REG__:      peek_reg(pid, fd);      break;
        case __NEXT_SYSCALL__:  next_syscall(pid, fd);  break;

        default:                printf("> unrecognised command .. awaiting another\n");
                                req.control = __UNRECOGN__;
                                req.value = __ANY__;
                                write(fd, &req, __REQ_SIZE__);
                                break;                  
    }
    goto loop;
    
    exit: err = kill(pid, SIGKILL);
    assert(err != -1, "killing tracee failed");
    printf("> EXIT\n");
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
    sockaddr_un_len = (socklen_t) strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

    // check if file already exists
    int err = access(__SOCKET_PATH__, F_OK);    // file exists -> err := zero
    assert_soft(err, "Socket File does exist! Unlinking ... ");
    if(!err) unlink(__SOCKET_PATH__);           // unlinking in order to rebind

    err = bind(socket_fd, (struct sockaddr *) &server_addr, sockaddr_un_len);
    assert(err != -1, "Bind failed");

    printf("> listening ..");

    err = listen(socket_fd, 5);
    assert(err != -1, "Listen failed");

    fd = accept(socket_fd, (struct sockaddr *) &client_addr, &sockaddr_un_len);
    assert(fd != -1, "Accept failed");

    printf(".. Connection established\n");

    return fd;
}

pid_t init_tracee(char *str)
{
    //convert string to tokens
    int counter = 1;
    for(int i = 0; str[i] != 0x0; i++)
    {
        if(str[i] == 32) counter++;
    }
    char **tokens = malloc(sizeof(void *) * counter);
    counter = 0;
    tokens[0] = str + counter;
    for(int i = 0; str[i] != 0x0; i++)
    {
        if(str[i] == 32)
        {
            tokens[++counter] = str + i + 1;
            str[i] = 0x0;
        }
    }

    // create tracee
    pid_t pid;
    int status, err;
    pid = fork();
    assert(pid != -1, "fork failed");

    if(!(pid)) // child
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        // close(STDOUT_FILENO); // currently we are writing stdout to log.txt anyway
        raise(SIGSTOP);
        err = execvp(tokens[0], tokens);
        exit(EXIT_FAILURE);
    }

    // wait for sys 14
    // waitpid(pid, &status, 0);
    // assert(!WIFEXITED(status), "sys 14 failed");
    // ptrace(PTRACE_SYSCALL, pid, 0,0);
    // waitpid(pid, &status, 0);
    // assert(!WIFEXITED(status), "sys 14 failed");
    // ptrace(PTRACE_SYSCALL, pid, 0,0);

    // wait for execvp
    waitpid(pid, &status, 0);
    assert(!WIFEXITED(status), "execvp() failed");
    ptrace(PTRACE_SYSCALL, pid, 0,0);
    waitpid(pid, &status, 0);
    assert(!WIFEXITED(status), "execvp() failed");

    printf("> Tracee created, traced, and interrupted\n");
    return pid;
}

void init_log()
{
    FILE *log_out = fopen("log.txt", "w");
    int log_fd = fileno(log_out);
    close(1);           //closing stdout
    dup(log_fd);        //stdout is now written to "log.txt"      
    fclose(log_out);    // closing stream
    close(log_fd);      // closing File Descriptor
    // only stdout is open, and leads towards log.txt
    printf("> Log output redirected\n");
}