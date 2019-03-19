#include "cli.h"

int main()
{
    int err;
    pid_t pid = fork();
    if(pid==-1) // error
    {
        return EXIT_FAILURE;
    }
    if(pid)     // parent
    {
        waitpid(pid, NULL, 0);

        // ...
        
    }
    else        // child
    {
        // pipe stdout and stderr into logfile
        FILE* log = fopen("log.log", "w");
        int fd = fileno(log);   
        close(fileno(stdout));  //closing stdout
        dup(fd);                //stdout now redirects to log.log
        close(fileno(stderr));  //closing stderr
        dup(fd);                //stderr now redirects to log.log
        // can we close the stream?

        char** dd = malloc(sizeof(char*)*2);
        assert(dd);
        dd[0] = malloc(sizeof(char)*5);
        assert(dd[0]);
        dd[1] = NULL;
        strncpy(dd[0], "./dd", 5);
        execvp(dd[0], dd);
        exit(EXIT_FAILURE);
    }
}