#include <stdio.h>
#include <czmq.h>

int main()
{
    zsock_t* sock= zsock_new_rep("tcp://192.168.178.33:3000");
    char* str;
    loop:
    str = zstr_recv(sock);
    puts(str);
    zstr_sendf(sock, "you sent '%s'",str);
    free(str);
    goto loop;
    zsock_destroy(&sock);
    return 0;
}
