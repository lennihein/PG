#include "cli.h"

#define TARGET "./tracee"

int main()
{
    int err;
    pid_t pid = fork();
    if(pid==-1) // error
    {
        return EXIT_FAILURE;
    }
    if(pid)     // parent
    {
        cli_routine();

        exit(EXIT_SUCCESS);

    }
    else        // child
    {
        // pipe stdout and stderr into logfile
        FILE* log = fopen("log.log", "w");
        int fd = fileno(log);   
        close(fileno(stdout));  //closing stdout
        dup(fd);                //stdout now redirects to log.log
        close(fileno(stderr));  //closing stderr
        dup(fd);                //stderr now redirects to log.log
        // can we close the stream?

        char** dd = malloc(sizeof(char*)*2);
        assert(dd);
        dd[0] = malloc(sizeof(char)*5);
        assert(dd[0]);
        dd[1] = NULL;
        strncpy(dd[0], "./dd", 5);
        execvp(dd[0], dd);
        exit(EXIT_FAILURE);
    }
}

void cli_routine()
{
    char* target = TARGET;
    fprintf(stderr, "now init...");
    zsock_t* sock = init(target);
    uint64_t rax = peek_reg(sock, RAX);
    destroy(sock); 
}

zsock_t* init(char* target)
{
    // 
    int err;
    zsock_t* sock = zsock_new(ZMQ_REQ);
    assert(sock);
    err = zsock_connect(sock, "tcp://127.0.0.1:5555");
    assert(err!=-1);

    // 
    fprintf(stderr, "sending target... ");
    err = zstr_send(sock, target);
    assert(err!=-1);
    char* str;
    str = zstr_recv(sock);
    assert(str);
    assert(!strcmp(str, "START"));
    free(str);

    fprintf(stderr, "done\n");

    return sock;
}

uint64_t peek_reg(zsock_t* sock, char* reg)
{
    int err = zstr_send(sock, "PEEK_REG");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, reg);
    assert(err!=-1);
    string = zstr_recv(sock);
    assert(string);
    uint64_t erg = strtoull(string, NULL, 10);
    free(string);
    return erg;
}

void destroy(zsock_t* sock)
{
    int err = zstr_send(sock, "EXIT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    assert(string);
    assert(!strcmp(string, "EXIT"));
    free(string);
    sleep(1);    
    zsock_destroy(&sock);
}