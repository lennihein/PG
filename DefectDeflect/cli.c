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
    getchar();
    int err = zstr_send(sock, "SINGLESTEP");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    assert(string);
    puts(string);
    free(string); 
    sleep(1);    
    zsock_destroy(&sock);
    return;

    char* input;
    scanf("%s", input);
    if(!strcmp(input,"PEEK_REG")) //peek_reg
    {
        char* reg;
        printf("type reg\n");
        scanf("%s",reg);
        uint64_t result = peek_reg(sock, reg);
        printf("%s: %lu",reg, result);
    }
    if(!strcmp(input,"POKE_REG")) //poke_reg
    {
        char* reg;
        uint64_t data;
        printf("type reg\n");
        scanf("%s", reg);
        printf("type data\n");
        scanf("%lu", &data);
        poke_reg(sock, reg, data);
        printf("poked succesfully\n");

    }
    if(!strcmp(input, "VIEW_STACK")) //view_stack
    {
        char* string=view_stack(sock);
        printf("%s",string);
        free(string);
    }
    if(!strcmp(input, "PEEK_ADR")) //peek_adr
    {
        uint64_t adr;
        printf("type reg\n");
        scanf("%lu",&adr);
        uint64_t result = peek_reg(sock, (char*)adr);
        printf("%lu: %lu\n",adr, result);
    }
    if(!strcmp(input, "RAISE_SIGNAL"))      //raise signal
    {
        int signal;
        printf("type signal number\n");
        scanf("%i",&signal);
        raise_signal(sock, signal);
        printf("signal raised\n");
    }
    if(!strcmp(input, "CREATE_BREAKPOINT")) // create breakpoint
    {
        uint64_t adr;
        printf("type adr\n");
        scanf("%lu",&adr);
        create_breakpoint(sock, adr);
        printf("breakpoint set\n");
    }
    if(!strcmp(input, "SHOW_BREAKPOINTS"))  // show breakpoints
    {
        char* string=show_breakpoints(sock);
        printf("%s",string);
        free(string);
    }
    if(!strcmp(input, "REMOVE_BREAKPOINT")) // remove breakpoint
    {
        uint64_t adr;
        printf("type adr\n");
        scanf("%lu",&adr);
        remove_breakpoint(sock,adr);
        printf("breakpoint removed\n");
    }
    if(!strcmp(input, "SINGLESTEP"))    // singlestep
    {
        if("EXIT"==singlestep(sock))    // singlestep reached end
        {
            destroy(sock);              
        }
    }
    if(!strcmp(input, "NEXT_SYSCALL"))   // next syscall
    {
        char* string=singlestep(sock);
        if("EXIT"==string)    // next_syscall reached end
        {
            destroy(sock);      
        }
        else                
        {
            char* buf=cutoff(string);
            if("ENTER"==buf)            // syscall entry
            {
                printf("syscall entry\n");
            }
            if("LEAVE"==buf)
            {
                printf("syscall exit\n"); // syscall exit
            }
            if("ENTER"!=buf&&"LEAVE"!=buf)
            {
                printf("unknown return\n");   // unexpected string
            }
        }
    }
    if(!strcmp(input, "INJECT_INSTRUCTIONS"))
    {
        uint64_t adr;
        char* payload;
        printf("type adr\n");
        scanf("%lu",&adr);
        printf("type payload\n");
        scanf("%s",payload);
        inject_instructions(sock, adr, payload);
        printf("instructions injected\n");
    }
    if(!strcmp(input, "CONTINUE"))
    {
        char* string=singlestep(sock);
        if("EXIT"==string)    // reached end of code
        {
            destroy(sock);      
        }
        else
        {
            printf("tracee continued\n");
        }
    }
    if(!strcmp(input, "EXIT"))
    {
        printf("shutting down ...");
    }
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

char* func_continue(zsock_t* sock)
{
    int err = zstr_send(sock, "CONTINUE");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    return string;
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
    string=cutoff(string);
    uint64_t erg = strtoull(string, NULL, 10);
    free(string);
    return erg;
}

uint64_t peek_adr(zsock_t* sock, char* adr)
{
    int err = zstr_send(sock, "PEEK_ADR");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, adr);
    assert(err!=-1);
    string = zstr_recv(sock);
    assert(string);
    string=cutoff(string);
    uint64_t erg = strtoull(string, NULL, 10);
    free(string);
    return erg;
}

void poke_reg(zsock_t* sock, char* reg, uint64_t data)
{
    int err = zstr_send(sock, "POKE_REG");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, reg);
    assert(err!=-1);
    string = zstr_recv(sock);
    err = zstr_send(sock, (char*)data);
    assert(err!=-1);
    string = zstr_recv(sock);
    string=cutoff(string);
    free(string);

}

void poke_adr(zsock_t* sock, uint64_t adr, uint64_t data)
{
    int err = zstr_send(sock, "POKE_ADR");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, (char*)adr);
    assert(err!=-1);
    string = zstr_recv(sock);
    err = zstr_send(sock, (char*)data);
    assert(err!=-1);
    string = zstr_recv(sock);
    string=cutoff(string);
    free(string);

}

void create_breakpoint(zsock_t* sock, uint64_t adr)
{
    int err = zstr_send(sock, "CREATE_BREAKPOINT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, (char*)adr);
    assert(err!=-1);
    string = zstr_recv(sock);
    free(string);

}

char* show_breakpoints(zsock_t* sock)
{
    int err = zstr_send(sock, "SHOW_BREAKPOINTS");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    string=cutoff(string);
    return string;
}

void remove_breakpoint(zsock_t* sock, uint64_t adr)
{
    int err = zstr_send(sock, "REMOVE_BREAKPOINT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, (char*)adr);
    assert(err!=-1);
    string = zstr_recv(sock);
    free(string);

}

char* singlestep(zsock_t* sock)
{
    int err = zstr_send(sock, "SINGLESTEP");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    return string;
}

char* next_syscall(zsock_t* sock)
{
    int err = zstr_send(sock, "NEXT_SYSCALL");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    return string;
}

void inject_instructions(zsock_t* sock, uint64_t adr, char* payload)
{
    int err = zstr_send(sock, "INJECT_INSTRUCTIONS");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, (char*)adr);
    assert(err!=-1);
    string = zstr_recv(sock);
    err = zstr_send(sock, payload);
    assert(err!=-1);
    string = zstr_recv(sock);
    string=cutoff(string);
    free(string);

}

char* view_stack(zsock_t* sock)
{
    int err = zstr_send(sock, "VIEW_STACK");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    assert(string);
    return cutoff(string); 
    
}

void raise_signal(zsock_t* sock, int signal)
{
    int err = zstr_send(sock, "RAISE_SIGNAL");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    assert(string);
    char* buf;
    sprintf(buf,"%i",signal);
    err = zstr_send(sock, buf);
    string = zstr_recv(sock);
    assert(string);
}

void func_exit(zsock_t* sock)
{
    int err = zstr_send(sock, "EXIT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
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

char* cutoff(char* string)  // cuts off "RETURN " at beginning
{
    if(strncmp(string, "RETURN ", 7))   // check if not 'RETURN '
    {
        free(string);
        return NULL;    // exit if unexpected return
    }
    else
    {        
        size_t len = strlen(string)+1-7;
        char* result = malloc(sizeof(char)*strlen(string)+1-7);
        strncpy(result, string+7, len);
        free(string);
        return result;  // return string without "RETURN "
    }
}