#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <czmq.h>
#include <inttypes.h>
#include <wait.h>
#include <assert.h>

void cli_routine();
zsock_t* init(char* target);
uint64_t peek_reg(zsock_t* sock, char* reg);
void destroy(zsock_t* sock);
char* view_stack(zsock_t* sock);
void poke_reg(zsock_t* sock, char* reg, uint64_t data);
char* cutoff(char* string);
uint64_t peek_adr(zsock_t* sock, char* adr);
void poke_adr(zsock_t* sock, uint64_t adr, uint64_t data);
void raise_signal(zsock_t* sock, int signal);
void create_breakpoint(zsock_t* sock, char* addr);
char* show_breakpoints(zsock_t* sock);
void remove_breakpoint(zsock_t* sock, char* addr);
char* singlestep(zsock_t* sock);
char* next_syscall(zsock_t* sock);
void inject_instructions(zsock_t* sock, char* payload);
char* func_continue(zsock_t* sock);
void func_exit(zsock_t* sock);
void convert(char* string);
char* convertreg(char* reg);

# define R15	    "0"
# define R14	    "8"
# define R13	    "16"
# define R12	    "24"
# define RBP	    "32"
# define RBX	    "40"
# define R11	    "48"
# define R10	    "56"
# define R9	        "64"
# define R8	        "72"
# define RAX	    "80"
# define RCX	    "88"
# define RDX	    "96"
# define RSI	    "104"
# define RDI	    "112"
# define ORIG_RAX   "120"
# define RIP	    "128"
# define CS	        "136"
# define EFLAGS	    "144"
# define RSP	    "152"
# define SS	        "160"
# define FS_BASE    "168"
# define GS_BASE    "176"
# define DS	        "184"
# define ES	        "192"
# define FS	        "200"
# define GS	        "208"