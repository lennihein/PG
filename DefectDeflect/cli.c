#include "cli.h"

int main(int argc, char **argv) 
{
    char* target = argv[1];
    // check for arguments
    if(argc == 3)
    {
        if(strcmp(argv[2], "--standalone")==0)
        {
            printf("Standalone mode\n\n");
            cli_routine(target);
            return EXIT_SUCCESS;
        }
        printf("wut");
    }
    int err;
    pid_t pid = fork();
    if(pid==-1) // error
    {
        return EXIT_FAILURE;
    }
    if(pid)     // parent
    {
        cli_routine(target);
        waitpid(pid, NULL, 0);
        exit(EXIT_SUCCESS);

    }
    else        // child
    {
        // pipe stdout and stderr into logfile
        FILE* log = fopen("log.log", "w");
        int fd = fileno(log);   
        close(fileno(stdout));  // closing stdout
        dup(fd);                // stdout now redirects to log.log
        close(fileno(stderr));  // closing stderr
        dup(fd);                // stderr now redirects to log.log
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

void cli_routine(char* target)  // loop of instruction input
{
    fprintf(stderr, "now init...");
    zsock_t* sock = init(target);
    
    char* input = malloc(sizeof(char)*26);

    printf("\n=================================================\n\n");
    printf("Type one of the following options:\n");
    printf("continue, exit,\n");
    printf("peek_reg, poke_reg, peek_address, poke_address,\n");
    printf("create_breakpoint, remove_breakpoint, show_breakpoints,\n");
    printf("singlestep, next_syscall,\n");
    printf("view_stack, inject_instructions, raise_signal\n\n");
    printf("'help' for list of commands\n");

    loop:   // begin of loop

    printf("\n");
    scanf("%25s", input);
    convert(input);
    if(!strcmp(input,"HELP"))   // list of commands
    {
        printf("\n=================================================\n\n");
        printf("Type one of the following options:\n");
        printf("continue, exit,\n");
        printf("peek_reg, poke_reg, peek_address, poke_address,\n");
        printf("create_breakpoint, remove_breakpoint, show_breakpoints,\n");
        printf("singlestep, next_syscall,\n");
        printf("view_stack, inject_instructions, raise_signal\n\n");
        goto loop;
    }
    if(!strcmp(input,"PEEK_REG"))
    {
        char* reg = malloc(sizeof(char)*9);
        printf("Type the register you want to peek in\n");
        scanf("%s",reg);
        convert(reg);                   // lower to upper case
        strcpy(reg,convertreg(reg));    // char* to int
        if(!strcmp(reg,"ERROR"))        // if unknown input
        {
            printf("not a register\n");
            free(reg);
            goto loop;
        }
        uint64_t result = peek_reg(sock, reg);
        printf("%s: %lu\n",reg, result);
        free(reg);
        goto loop;
    }
    if(!strcmp(input,"POKE_REG"))
    {
        char* reg = malloc(sizeof(char)*8);
        char* data = malloc(sizeof(char)*21);
        printf("Type the register you want to overwrite\n");
        scanf("%s", reg);
        convert(reg);                   // lower to upper case
        strcpy(reg,convertreg(reg));    // char* to int
        if(!strcmp(reg,"ERROR"))        // if unknow input
        {
            printf("not a register\n");
            free(reg);
            goto loop;
        }
        printf("Type the data you want to overwrite\n");
        scanf("%s", data);
        poke_reg(sock, reg, data);
        printf("Poke succesful\n");
        free(reg);
        free(data);
        goto loop;

    }
    if(!strcmp(input, "VIEW_STACK"))
    {
        char* string=view_stack(sock);
        printf("%s\n",string);      // print entire stack frame
        free(string);
        goto loop;
    }
    if(!strcmp(input, "PEEK_ADDRESS")) 
    {
        char* addr = malloc(sizeof(char)*20);
        printf("Type the address you want to overwrite\n");
        scanf("%s",addr);
        uint64_t result = peek_addr(sock,addr);
        printf("%s: %lu\n",addr, result);
        free(addr);
        goto loop;
    }
    if(!strcmp(input, "POKE_ADDRESS")) 
    {
        char* addr = malloc(sizeof(char)*20);
        char* data = malloc(sizeof(char)*21);
        printf("Type the adress you want to overwrite\n");
        scanf("%s", addr);
        printf("Type the data you want to overwrite\n");
        scanf("%s", data);
        poke_reg(sock, addr, data);
        printf("Poke succesful\n");
        free(addr);
        free(data);
        goto loop;
    }
    if(!strcmp(input, "RAISE_SIGNAL"))     
    {
        int signal;
        printf("Type the signal number\n");
        scanf("%i",&signal);
        raise_signal(sock, signal);
        printf("signal raised\n");
        goto loop;
    }
    if(!strcmp(input, "CREATE_BREAKPOINT")) 
    {
        char* addr = malloc(sizeof(char)*32);
        printf("Type the address\n");
        scanf("%31s",addr);
        create_breakpoint(sock, addr);
        printf("breakpoint set\n");
        free(addr);
        goto loop;
    }
    if(!strcmp(input, "SHOW_BREAKPOINTS"))  
    {
        char* string = malloc(sizeof(char)*256);
        string=show_breakpoints(sock);
        printf("%s",string);
        free(string);
        goto loop;
    }
    if(!strcmp(input, "REMOVE_BREAKPOINT")) 
    {
        char* addr = malloc(sizeof(char)*32);
        printf("Type the address\n");
        scanf("%31s",addr);
        remove_breakpoint(sock,addr);
        printf("breakpoint removed\n");
        free(addr);
        goto loop;
    }
    if(!strcmp(input, "SINGLESTEP"))   
    {
        if(!strcmp("EXIT",singlestep(sock)))    // singlestep reached end
        {
            zsock_destroy(&sock);
            free(input);
            return;
        }
        printf("stepped\n");
        goto loop;
    }
    if(!strcmp(input, "NEXT_SYSCALL"))  
    {
        char* string = malloc(sizeof(char)*8);
        string=next_syscall(sock);
        if(!strcmp("EXIT",string))    // next_syscall reached end
        {
            zsock_destroy(&sock);
            free(string);
            free(input);
            return;  
        }
        else                
        {
            char* buf = malloc(sizeof(char)*16);
            buf=cutoff(string);
            if(!strcmp("ENTER",buf))            // syscall entry
            {
                printf("syscall entry\n");
            }
            if(!strcmp("LEAVE",buf))
            {
                printf("syscall exit\n"); // syscall exit
            }
            if(strcmp("ENTER",buf)&&strcmp("LEAVE",buf)) // if neither ENTER nor LEAVE
            {
                printf("unknown return\n");   // unexpected string, shouldnt even happen
            }
        }
        goto loop;
    }
    if(!strcmp(input, "INJECT_INSTRUCTIONS"))   
    {
        char* payload = malloc(sizeof(char)*256);
        printf("Type the payload you want to insert at RIP\n");
        scanf("%s",payload);
        inject_instructions(sock, payload);
        printf("instructions injected\n");
        goto loop;
    }
    if(!strcmp(input, "CONTINUE"))   
    {
        char* string = malloc(sizeof(char)*10);
        string=func_continue(sock);
        if(!strcmp(string, "EXIT"))    // reached end of code
        {
            free(string);
            zsock_destroy(&sock);
            return;    
        }
        else
        {
            free(string);
            printf("tracee continued\n");
        }        
        goto loop;
    }
    if(!strcmp(input, "EXIT"))     
    {
        printf("shutting down ...");
        free(input);
        destroy(sock); 
        return;
    }
    printf("WRONG INPUT\n");    // if none of the above
    goto loop;
}

zsock_t* init(char* target)
{
    // connecting to session
    int err;
    zsock_t* sock = zsock_new(ZMQ_REQ);
    assert(sock);
    err = zsock_connect(sock, "tcp://127.0.0.1:5555");
    assert(err!=-1);

    // sending tracee name
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

void destroy(zsock_t* sock) // stops connection
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

// data transfer between debugger and cli for each function
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
    string = cutoff(string);
    uint64_t erg = strtoull(string, NULL, 10);
    free(string);
    return erg;
}

uint64_t peek_addr(zsock_t* sock, char* addr) 
{
    int err = zstr_send(sock, "PEEK_ADDRESS");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, addr);
    assert(err!=-1);
    string = zstr_recv(sock);
    assert(string);
    string=cutoff(string);
    uint64_t erg = strtoull(string, NULL, 10);
    free(string);
    return erg;
}

void poke_reg(zsock_t* sock, char* reg, char* data) 
{
    int err = zstr_send(sock, "POKE_REG");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, reg);
    assert(err!=-1);
    string = zstr_recv(sock);
    err = zstr_send(sock, data);
    assert(err!=-1);
    string = zstr_recv(sock);
    free(string);

}

void poke_addr(zsock_t* sock, char* addr, char* data)   
{
    int err = zstr_send(sock, "POKE_ADDRESS");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, addr);
    assert(err!=-1);
    string = zstr_recv(sock);
    err = zstr_send(sock, data);
    assert(err!=-1);
    string = zstr_recv(sock);
    string=cutoff(string);
    free(string);

}

void create_breakpoint(zsock_t* sock, char* addr)  
{
    int err = zstr_send(sock, "CREATE_BREAKPOINT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, addr);
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

void remove_breakpoint(zsock_t* sock, char* addr) 
{
    int err = zstr_send(sock, "REMOVE_BREAKPOINT");
    assert(err!=-1);
    char* string = zstr_recv(sock);
    err = zstr_send(sock, addr);
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

void inject_instructions(zsock_t* sock, char* payload) 
{
    int err = zstr_send(sock, "INJECT_INSTRUCTIONS");
    assert(err!=-1);
    char* string = zstr_recv(sock);
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
    char* string = malloc(sizeof(char)*256);
    string = zstr_recv(sock);
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

void convert(char* string) // small to caps and spaces to _ "peek reg" to "PEEK_REG"
{
    for(int i=0; i<strlen(string); i++)
    {
        if((int)string[i]!=95&&(int)string[i]>=97&&(int)string[i]<=122) // check if not "_" and in range of ascii number a-z
        {
            string[i]=(char)((int)string[i]-32); // ascii number lower to upper case
        }
    }
}

char* convertreg(char* reg) // converts register string to int (RAX to 80)
{
    char* str = malloc(sizeof(char)*9);
    // check for reg as string
    if(!strcmp(reg,"R15")){return "0";}
    if(!strcmp(reg,"R14")){return "8";}
    if(!strcmp(reg,"R13")){return "16";}
    if(!strcmp(reg,"R12")){return "24";}
    if(!strcmp(reg,"RBP")){return "32";}
    if(!strcmp(reg,"R11")){return "40";}
    if(!strcmp(reg,"R10")){return "48";}
    if(!strcmp(reg,"R9")){return "56";}
    if(!strcmp(reg,"R8")){return "72";}
    if(!strcmp(reg,"RAX")){return "80";}   
    if(!strcmp(reg,"RCX")){return "88";}
    if(!strcmp(reg,"RDX")){return "96";} 
    if(!strcmp(reg,"RSI")){return "104";}
    if(!strcmp(reg,"RDI")){return "112";}
    if(!strcmp(reg,"ORIG_RAX")){return "120";}
    if(!strcmp(reg,"RIP")){return "128";}
    if(!strcmp(reg,"CS")){return "136";}
    if(!strcmp(reg,"EFLAGS")){return "144";}
    if(!strcmp(reg,"RSP")){return "152";}
    if(!strcmp(reg,"SS")){return "160";}
    if(!strcmp(reg,"FS_BASE")){return "168";}
    if(!strcmp(reg,"GS_BASE")){return "176";}
    if(!strcmp(reg,"DS")){return "184";}
    if(!strcmp(reg,"ES")){return "192";}
    if(!strcmp(reg,"FS")){return "200";}
    if(!strcmp(reg,"GS")){return "208";}
    // check for reg as int
    if(!strcmp(reg,"0")){return "0";}   // check for 0
    char* ptr;
    long num = strtol(reg,&ptr,10); // convert to int
    if(num==0){return "ERROR";}      // if not possible num=0;
    if(num>0&&num<=208&&num%8==0){return reg;}  // check if actual reg number
}
