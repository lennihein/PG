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
#include <inttypes.h>   // uint64_t

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

zsock_t* init_net();
pid_t init_tracee(char* str);

// functionalities of the API and their underlying wrapped implementation
void func_exit(zsock_t*, pid_t);
void __exit__(pid_t pid);
void func_continue(zsock_t*, pid_t);
int __continue__(pid_t pid);
void func_create_breakpoint(zsock_t*, pid_t);
void __create_breakpoint__(pid_t pid, uint64_t addr, std::map<uint64_t, uint64_t>* breakpoints_ptr);
void func_show_breakpoints(zsock_t*, pid_t);
void __create_breakpoint__(pid_t pid, std::map<uint64_t, uint64_t>* breakpoints_ptr);
void func_remove_breakpoint(zsock_t*, pid_t);
void __create_breakpoint__(pid_t pid, uint64_t addr, std::map<uint64_t, uint64_t>* breakpoints_ptr);
void func_singlestep(zsock_t*, pid_t);
int __singlestep__(pid_t pid);
void func_next_syscall(zsock_t*, pid_t);
int __next_syscall__(pid_t pid);
void func_inject_instructions(zsock_t*, pid_t);
// todo: DD3
void func_poke_reg(zsock_t*, pid_t);
void __poke_reg__(pid_t pid, uint64_t reg, uint64_t data);
void func_peek_reg(zsock_t*, pid_t);
uint64_t __peek_reg__(pid_t pid, uint64_t reg);
void func_peek_address(zsock_t*, pid_t);
uint64_t __peek_addr__(pid_t pid, uint64_t addr);
void func_poke_address(zsock_t*, pid_t pid);
void __poke_addr__(pid_t pid, uint64_t addr, uint64_t data);
void func_view_stack(zsock_t*, pid_t);
char* __view_stack__(pid_t pid);
void func_raise_signal(zsock_t*, pid_t);
void __raise_signal__(pid_t pid, int signal);
// todo: DD4 - step in/step over