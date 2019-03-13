#include "header.hpp"

#define TARGET "./tracee"
// #define TARGET "./a.out"

int main()
{   
    int err;
    zsock_t* sock = zsock_new(ZMQ_REQ);
    assert(sock);
    err = zsock_connect(sock, "tcp://127.0.0.1:5555");
    assert_no_err(err);
    err = zstr_send(sock, TARGET);
    assert_no_err(err);
    zstr_recv(sock);
    char* string;

    // poke 12 into rdx
    err = zstr_send(sock, "POKE_REG");
    assert_no_err(err);
    string = zstr_recv(sock);
    err = zstr_send(sock, "96");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    free(string);
    err = zstr_send(sock, "12");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    free(string);

    // get rdx
    err = zstr_send(sock, "PEEK_REG");
    assert_no_err(err);
    string = zstr_recv(sock);
        err = zstr_send(sock, "96");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    printf("RDX: %s\n", string);
    free(string);

    err = zstr_send(sock, "NEXT_SYSCALL");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    puts(string);
    free(string);

    loop:

    // get rax
    err = zstr_send(sock, "PEEK_REG");
    assert_no_err(err);
    string = zstr_recv(sock);
    err = zstr_send(sock, "80");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    printf("RAX: %s\n", string);
    free(string);

    // get orig_rax
    err = zstr_send(sock, "PEEK_REG");
    assert_no_err(err);
    string = zstr_recv(sock);
        err = zstr_send(sock, "120");
    assert_no_err(err);
    string = zstr_recv(sock);
    assert(string);
    printf("ORIG_RAX: %s\n", string);
    free(string);

    err = zstr_send(sock, "NEXT_SYSCALL");
    assert_no_err(err);

    string = zstr_recv(sock);
    assert(string);
    puts(string);
    if(!strcmp(string, "EXIT")) goto exit;
    free(string);
    goto loop;
    
    err = zstr_send(sock, "EXIT");
    assert_no_err(err);

    string = zstr_recv(sock);
    assert(string);
    puts(string);
    free(string);

    sleep(1);

    exit: zsock_destroy(&sock);
    return EXIT_SUCCESS;
}