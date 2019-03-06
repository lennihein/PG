#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <czmq.h>
#include <signal.h>

#include "constants.hpp"

// #define NDEBUG   // uncomment for final build
#include <assert.h> // assertions

#define assert_null(STATEMENT) assert(!(STATEMENT))
#define assert_no_err(STATEMENT) assert(!(STATEMENT==-1))
#define assert_not_null(STATEMENT) assert(STATEMENT)

// credit to David Sorkovsky from SO 
// (https://stackoverflow.com/questions/3046889/)
#ifndef NDEBUG 

#define XXX_0()     fprintf(stderr, "Boop from %s:%i\n", __FILE__, __LINE__)
#define XXX_1(A)    XXX_0();fprintf(stderr, "%s: %d\n", #A, A)

#define boop_str(STR) XXX_0();fprintf(stderr, "%s: %s\n", #STR, STR)

#define XXX_X(x,A,FUNC, ...)  FUNC  

#define boop(...)   XXX_X(,##__VA_ARGS__,\
                    XXX_1(__VA_ARGS__),\
                    XXX_0(__VA_ARGS__)) 

#endif

void init_log();
zsock_t* init_net();
pid_t init_tracee(char* str);

void func_exit(zsock_t*, pid_t);
void __exit__(pid_t);
void func_continue(zsock_t*, pid_t);
int __continue__(pid_t);
void func_create_breakpoint(zsock_t*, pid_t);

void func_show_breakpoints(zsock_t*, pid_t);

void func_remove_breakpoint(zsock_t*, pid_t);

void func_singlestep(zsock_t*, pid_t);

void func_next_syscall(zsock_t*, pid_t);

void func_inject_instructions(zsock_t*, pid_t);

void func_poke_reg(zsock_t*, pid_t);

void func_peek_reg(zsock_t*, pid_t);

void func_peek_address(zsock_t*, pid_t);

void func_poke_address(zsock_t*, pid_t);

void func_view_stack(zsock_t*, pid_t);

void func_raise_signal(zsock_t*, pid_t);