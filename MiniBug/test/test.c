#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>

int main()
{
    printf("%lu\n", sizeof(long));
    printf("%lu\n", sizeof(unsigned long));
    printf("%lu\n", sizeof(long long));
    printf("%lu\n", sizeof(unsigned long));
    printf("%lu\n", sizeof(int64_t));
    printf("%lu\n", sizeof(uint64_t));

    printf("%lu\n", sizeof(uint8_t));
    printf("%lu\n", sizeof(uint16_t));
    // uint8_t control;
    // uint16_t value;
    exit(EXIT_SUCCESS);
}