// std
#include <stdio.h>
#include <stdlib.h>

// Sockets
#include <sys/socket.h>
#include <sys/types.h>

// TCP
#include <netinet/in.h>
#include <arpa/inet.h>

// UNIX
#include <sys/un.h>

#include <unistd.h>

// misc
#include <string.h>

// defines
#define __PORT__ 4711
#define __SOCKET_PATH__ "tmp_sock"
#define __MSG_SIZE__ 64

// assert
#define assert(expr, msg) \
    if (!(expr)) \
        __assertion_failed__(__FILE__, __LINE__, msg, 1)
    
#define assert_soft(expr, msg) if(!(expr))__assertion_failed__(__FILE__,__LINE__,msg,0)

void __assertion_failed__(char* file, int line, char* msg, int do_exit)
{
    fprintf(stderr,         "ASSERT%s: %s : %i\n", do_exit?"  ":"(soft)", file, line);
    if(msg) fprintf(stderr, "MESSAGE%s: %s\n", do_exit?" ":"     ",msg);
    if(do_exit) exit(EXIT_FAILURE);
}