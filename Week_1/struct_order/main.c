#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct test
{
    uint64_t a;
    uint64_t b;
} test;

void launch();

int main(void)
{
    uint64_t x = 'x';
    uint64_t y = 'y';
    printf("x: %p\n", &x);
    printf("y: %p\n", &y);
    launch();
    test test;
    test.a = 'a';
    test.b = 'b';
    printf("a: %p\n", &test.a);
    printf("b: %p\n", &test.b);
    printf("t: %p\n", &test);
    return EXIT_SUCCESS;
}

void launch()
{
    uint64_t l = 'l';
    printf("l: %p\n", &l);    
}