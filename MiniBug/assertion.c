#include "assertion.h"

void __assertion_failed__(char *file, int line, char *msg, int do_exit)
{
    fprintf(stderr, "ASSERT%s: %s : %i\n", do_exit ? "  " : "(soft)", file, line);
    if(msg) fprintf(stderr, "MESSAGE%s: %s\n", do_exit ? " " : "     ", msg);
    if(do_exit) exit(EXIT_FAILURE);
}

void print_bytes(void *ptr, int size)
{
    unsigned char *p = ptr;
    int i;
    for (i=0; i<size; i++) {
        printf("0x%02hhX ", p[i]);
    }
    printf("\n");
}