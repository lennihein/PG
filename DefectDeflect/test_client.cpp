#include "header.hpp"

#define TARGET "./tracee"

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

    err = zstr_send(sock, "CONTINUE");
    assert_no_err(err);

    char* string = zstr_recv(sock);
    assert(string);
    puts(string);
    free(string);

    err = zstr_send(sock, "EXIT");
    assert_no_err(err);

    string = zstr_recv(sock);
    assert(string);
    puts(string);
    free(string);

    sleep(1);

    zsock_destroy(&sock);
    return EXIT_SUCCESS;
}