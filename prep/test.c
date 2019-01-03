#include <stdio.h>

void test();

int main () 
{
    test();
}

void test()
{
    unsigned long rbp;
    asm("leaq (%%rbp), %0;": "=r"(rbp));
    printf("%lx\n", rbp);
}