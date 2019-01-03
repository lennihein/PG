/*
*   Functions
*   
*   every function has the same signature (apart from the name).
*
*   void FUNCTION_NAME(pid_t pid, int fd);
*
*/

#include "api.h"
#include "khh.h"

void peek_reg(pid_t pid, int fd);
void next_syscall(pid_t, int fd);