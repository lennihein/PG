#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig);

int main()
{
    printf("PID : %d\n", getpid());
    if(signal(SIGINT, sig_handler) == SIG_ERR) exit(1);
    int x = 0;
    loop:
    sleep(3);
    printf("Hey(%i)\n", x++);
    goto loop;
    return EXIT_SUCCESS;
}

void sig_handler(int sig)
{
    printf("Caught Signal\n");
    exit(EXIT_SUCCESS);
}