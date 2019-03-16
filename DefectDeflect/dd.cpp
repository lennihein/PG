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

    void* command;
    char* command_str;

    loop: // main loop

    command_str = zstr_recv(sock);
    command = (void*) func[command_str];
    if(command == NULL)
    {
        fprintf(stderr, "> %s - command not found\n", command_str);
        zstr_send(sock, "COMMAND_NOT_FOUND");
        __exit__(pid);
        sleep(1);
        zsock_destroy(&sock);    
        fprintf(stderr, "> DefectDeflect shutting down\n");
        exit(0);

    }
    else
    {
        printf("> %s\n", command_str);
        ((void (*)(zsock_t*, pid_t))command)(sock, pid);
    }

    goto loop;
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
        // close(STDOUT_FILENO); // would omit tracee output
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
    // skipped syscalls are sys_rt_sigprocmask and sys_execve
    for(int i = 0; i < 5; i++) 
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

// sends "RETURN" or "EXIT"
// based on type of int
void func_continue(zsock_t* sock, pid_t pid)
{
    int code = __continue__(pid);
    if(code == __EXIT__) func_exit(sock, pid);
    zstr_sendf(sock, "RETURN");
}

// sends ""
// receives an address (decimal to str)
// sends "RETURN"
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
// sends "RETURN""
void func_remove_breakpoint(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    // int code = __remove_breakpoint__(pid, atoi(str));
    int code = 0; // only stub
    zstr_sendf(sock, "RETURN %s", code?"FAILURE":"SUCCESS");
    free(str);
}

// sends "RETURN" or "EXIT" 
// based on type of ssoftware interrupt
void func_singlestep(zsock_t* sock, pid_t pid)
{
    int code = __singlestep__(pid);
    if(code == __EXIT__) func_exit(sock, pid);
    else zstr_sendf(sock, "RETURN");

}

// JUMPS PAST BREAKPOINTS
// sends "RETURN [ENTER/LEAVE]" or "EXIT"
// based on whether or not a next syscall was even existent
// and if it was the interrupt before or after the kernal handles the call
void func_next_syscall(zsock_t* sock, pid_t pid)
{
    int code = __next_syscall__(pid);
    if(code!=__EXIT__) zstr_sendf(sock, "RETURN %s", code?"LEAVE":"ENTER");
    else func_exit(sock, pid);
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
    char* end = str + strlen(str) - 1;
    uint64_t reg = strtoull(str, &end, 10);
    uint64_t res = __peek_reg__(pid, reg);
    zstr_sendf(sock, "RETURN %" PRIu64, res);
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
    char* end1 = str1 + strlen(str1) - 1;
    char* end2 = str2 + strlen(str2) - 1;
    uint64_t reg = strtoull(str1, &end1, 10);
    uint64_t data = strtoull(str2, &end2, 10);
    __poke_reg__(pid, reg, data);
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
    char* end = str + strlen(str) - 1;
    uint64_t addr = strtoull(str, &end, 10);
    uint64_t res = __peek_addr__(pid, addr);
    zstr_sendf(sock, "RETURN %" PRIu64, res);
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
    char* end1 = str1 + strlen(str1) - 1;
    char* end2 = str2 + strlen(str2) - 1;
    uint64_t addr = strtoull(str1, &end1, 10);
    uint64_t data = strtoull(str2, &end2, 10);
    __poke_addr__(pid, addr, data);
    zstr_send(sock, "RETURN");
    free(str1);
    free(str2);
}

// sends "RETURN <literally the whole fucking stack (hex to str)>"
void func_view_stack(zsock_t* sock, pid_t pid)
{
    char* stackframe = __view_stack__(pid);
    zstr_sendf(sock, "RETURN %s", stackframe);
    free(stackframe);
}

// sends ""
// receives signal (decimal to str, see signal.h)
// sends "RETURN"
void func_raise_signal(zsock_t* sock, pid_t pid)
{
    zstr_send(sock, "");
    char* str = zstr_recv(sock);
    __raise_signal__(pid, atoi(str));
    zstr_send(sock, "RETURN");
}