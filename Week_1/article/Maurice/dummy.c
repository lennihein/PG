#include <stdio.h>
#include <unistd.h>
int main()
{
    int i;
    for(i=0; i<10; i++)
    {
        printf("My counter is %d\n",i);
        sleep(2);
    }
    return 0;
}