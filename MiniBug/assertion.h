#include <stdio.h>
#include <stdlib.h>

#define assert(expr, msg) \
    if (!(expr)) \
        __assertion_failed__(__FILE__, __LINE__, msg, 1)

#define assert_soft(expr, msg) if(!(expr))__assertion_failed__(__FILE__,__LINE__,msg,0)

void __assertion_failed__(char *file, int line, char *msg, int do_exit);

void print_bytes(void *ptr, int size);