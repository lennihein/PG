#include "functions.h"

int init_net(); // initialises berkeley socket server

void init_log(); // closes `stdout` and redirects every output to `stdout` to `log.txt`

pid_t init_tracee(char *str); // starts and traces tracee

void debug_loop(pid_t pid, int fd);