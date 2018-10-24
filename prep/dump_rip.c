#include <stdio.h>
#include <stdlib.h>

int main()
{   
    unsigned long rip;
    asm("leaq (%%rip), %0;": "=r"(rip));
    printf("%lx\n", rip);
    return EXIT_SUCCESS;
}