#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <iterator> 
#include <string>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <czmq.h>
#include <signal.h>
#include <inttypes.h>

#include "constants.hpp"

// #define NDEBUG   // uncomment for final build
#include <assert.h> // assertions

zsock_t* init_net();
pid_t init_tracee(char* str);

// functionalities of the API and their underlying wrapped implementation
void func_exit(zsock_t*, pid_t);
void __exit__(pid_t pid);
void func_continue(zsock_t*, pid_t);
int __continue__(pid_t pid);
void func_create_breakpoint(zsock_t*, pid_t);
void __create_breakpoint__(pid_t pid, uint64_t addr);
void func_show_breakpoints(zsock_t*, pid_t);
char* __show_breakpoints__(pid_t pid);
void func_remove_breakpoint(zsock_t*, pid_t);
int __remove_breakpoint__(pid_t pid, uint64_t addr);
int __check_for_breakpoint__(pid_t pid);
void func_singlestep(zsock_t*, pid_t);
int __singlestep__(pid_t pid);
void func_next_syscall(zsock_t*, pid_t);
int __next_syscall__(pid_t pid);
void func_inject_instructions(zsock_t*, pid_t);
// todo: DD3 - inject instructions
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