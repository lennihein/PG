#include "header.hpp"

int main()
{   
    zsock_t* sock;
    pid_t pid;

    fprintf(stderr, "> DefectDeflect service started\n");

    // init_log();
    sock = init_net(); 

    // get target from Client and call init
    char *target = zstr_recv(sock);
    assert(target);
    printf("> target: %s\n", target);
    pid = init_tracee(target);
    free(target);
    zstr_send(sock, "START");

    // map all the functionalities
    std::map<std::string, void (*)(zsock_t*, pid_t)> func;

    func["EXIT"] = &func_exit;
    func["CONTINUE"] = &func_continue;
    func["CREATE_BREAKPOINT"] = &func_create_breakpoint;
    func["SHOW_BREAKPOINTS"] = &func_show_breakpoints;
    func["REMOVE_BREAKPOINT"] = &func_remove_breakpoint;
    func["SINGLESTEP"] = &func_singlestep;
    func["NEXT_SYSCALL"] = &func_next_syscall;
    func["INJECT_INSTRUCTIONS"] = &func_inject_instructions;
    func["POKE_REG"] = &func_poke_reg;
    func["PEEK_REG"] = &func_peek_reg;
    func["POKE_ADDRESS"] = &func_poke_address;
    func["PEEK_ADDRESS"] = &func_peek_address;
    func["VIEW_STACK"] = &func_view_stack;
    func["RAISE_SIGNAL"] = &func_raise_signal;

    printf("> functionalities mapped\n");

    loop: // main loop

    char* command = zstr_recv(sock);
    printf("> %s\n", command);

    func[command](sock, pid); 
    // todo what happens if command not mappable?

    goto loop;
}

// redirect stdout to file
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

// setup ZMQ connection
zsock_t* init_net() 
{
    int err;
    zsock_t* sock = zsock_new(ZMQ_REP);
    assert_not_null(sock);
    err = zsock_bind(sock, "tcp://127.0.0.1:5555");
    assert_no_err(err);
    printf("> Serving %s\n", ENDPOINT);
    return sock;
}

// run tracee process and trace
pid_t init_tracee(char *str)    // todo: revise
{
    //convert string to tokens
    int counter = 1;
    for(int i = 0; str[i] != 0x0; i++)
    {
        if(str[i] == 32) counter++;
    }
    char** tokens = (char**) malloc(sizeof(char*) * counter);
    tokens[0] = str;
    tokens[counter] = 0x0;
    counter = 0;
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
    assert(pid != -1);

    if(!(pid)) // child
    {
        ptrace(PTRACE_TRACEME, NULL, NULL, NULL);
        ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_EXITKILL);
        // close(STDOUT_FILENO); // currently we are writing stdout to log.txt anyway
        raise(SIGSTOP);
        err = execvp(tokens[0], tokens);
        exit(EXIT_FAILURE);
    }

    // wait for first interrupt of tracee
    waitpid(pid, &status, 0);

    // this one is weird, but it removes 
    // the clutter that results from syscalls 
    // that are due to the debugging process
    // so only the syscalls from the original
    // process are shown
    for(int i = 0; i < 6; i++)
    {
        ptrace(PTRACE_SYSCALL, pid, 0,0);
        waitpid(pid, &status, 0);
    }

    // clean up tokens from earlier
    for(int i=0; i<counter; i++)
    {
        free(tokens[i]);
    }
    free(tokens);

    printf("> Tracee created, traced, and interrupted\n");
    return pid;
}

// sends "EXIT"
void func_exit(zsock_t* sock, pid_t pid)
{
    __exit__(pid);
    zstr_send(sock, "EXIT");

    sleep(1);

    zsock_destroy(&sock);    
    fprintf(stderr, "> DefectDeflect shutting down\n");
    exit(0);
}

// sends "RETURN [BREAKPOINT/INT]" or "EXIT" 
// based on type of int
void func_continue(zsock_t* sock, pid_t pid)
{
    int code = __continue__(pid);
    if(code == -1) func_exit(sock, pid);
    zstr_sendf(sock, "RETURN %s", code?"BREAKPOINT":"INT");
}

// sends ""
// receives an address (decimal to str)
// sends "RETURN [SUCCESS/FAILURE]"
// based on whether or not a breakpoint was set
// note: 'FAILURE' in case of breakpoint already set at that addr
void func_create_breakpoint(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    // int code = __create_breakpoint__(pid, atoi(str));
    int code = 0; // only stub
    zstr_sendf(sock, "RETURN %s", code?"FAILURE":"SUCCESS");
    free(str);
}

// sends "RETURN <addr1> <addr2> ..."
void func_show_breakpoints(zsock_t* sock, pid_t pid)
{
    // zstr_sendf(sock, "RETURN %s", __show_breakpoints__(pid));
    zstr_send(sock, "RETURN");
}

// sends ""
// receives an address (decimal to str)
// sends "RETURN [SUCCESS/FAILURE]"
// based on whether or not a breakpoint was removed
void func_remove_breakpoint(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    // int code = __remove_breakpoint__(pid, atoi(str));
    int code = 0; // only stub
    zstr_sendf(sock, "RETURN %s", code?"FAILURE":"SUCCESS");
    free(str);
}

// sends "RETURN [STEP/BREAKPOINT/INT]" or "EXIT" 
// based on type of int
void func_singlestep(zsock_t* sock, pid_t pid)
{
    // int code = __singlestep__(pid);
    int code = 0; // only stub
    if(pid == -1) func_exit(sock, pid);
    zstr_sendf(sock, "RETURN %s", code?(code==1?"BREAKPOINT":"INT"):"STEP");

}

// JUMPS PAST BREAKPOINTS
// sends "RETURN" or "EXIT"
// based on whether or not a next syscall was even existent
void func_next_syscall(zsock_t* sock, pid_t pid)
{
    // int code = __next_syscall__(pid)
    int code = 0; // only stub
    zstr_sendf(sock, "%s", code?"RETURN":"EXIT");
}

// sends ""
// receives address (decimal to str)
// sends ""
// receives payload (decimal to str) 
// sends "RETURN"
void func_inject_instructions(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str1 = zstr_recv(sock);
    zstr_send(sock, "");
    char* str2 = zstr_recv(sock);
    // __inject__instructions(...)
    zstr_send(sock, "RETURN");
    free(str1);
    free(str2);
}

// sends ""
// receives reg name (int, see constants.hpp)
// sends "RETURN <reg_content>"
void func_peek_reg(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    // int res = __peek_reg__(..)
    int res = 42; // only stub
    zstr_sendf(sock, "RETURN %i", res);
    free(str);
}

// sends ""
// receives reg name (int, see constants.hpp)
// sends ""
// receives content (decimal to str)
// sends "RETURN"
void func_poke_reg(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str1 = zstr_recv(sock);
    zstr_send(sock, "");
    char* str2 = zstr_recv(sock);
    // ...
    zstr_send(sock, "RETURN");
    free(str1);
    free(str2);
}

// sends ""
// receives address (decimal to str)
// sends "RETURN <addr_content>"
void func_peek_address(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    // int res = ..
    int res = 43; // only stub
    zstr_sendf(sock, "RETURN %i", res);
    free(str);
}

// sends ""
// receives address (decimal to str)
// sends ""
// receives content (decimal to str)
// sends "RETURN"
void func_poke_address(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str1 = zstr_recv(sock);
    zstr_send(sock, "");
    char* str2 = zstr_recv(sock);
    // ...
    zstr_send(sock, "RETURN");
    free(str1);
    free(str2);
}

// sends "RETURN <literally the whole fucking stack (hex to str)>"
void func_view_stack(zsock_t* sock, pid_t pid)
{
    // char* stackframe = __view_stack__(pid);
    char* stackframe = (char*) malloc(sizeof(char));
    stackframe[0] = '\0';
    zstr_sendf(sock, "RETURN %s", stackframe);
    free(stackframe);
}

// sends ""
// receives signal (int, see signal.h)
// sends "RETURN"
void func_raise_signal(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    zstr_send(sock, "RETURN");
}