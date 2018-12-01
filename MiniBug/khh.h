// std
#include <stdio.h>
#include <stdlib.h>

// Sockets
#include <sys/socket.h>
#include <sys/types.h>

// TCP
#include <netinet/in.h>
#include <arpa/inet.h>

// UNIX
#include <sys/un.h>         // def. of `sockaddr_un`
#include <unistd.h>

// ptrace
#include <sys/ptrace.h>

// misc
#include <string.h>
#include <sys/wait.h>       // waitpid
#include <signal.h>

// defines
#define __PORT__ 4711
#define __SOCKET_PATH__ "tmp_sock"

#include "assertion.h"

// registers
# define R15	    8*00
# define R14	    8*01
# define R13	    8*02
# define R12	    8*03
# define RBP	    8*04
# define RBX	    8*05
# define R11	    8*06
# define R10	    8*07
# define R9	        8*08
# define R8	        8*09
# define RAX	    8*10
# define RCX	    8*11
# define RDX	    8*12
# define RSI	    8*13
# define RDI	    8*14
# define ORIG_RAX   8*15
# define RIP	    8*16
# define CS	        8*17
# define EFLAGS	    8*18
# define RSP	    8*19
# define SS	        8*20
# define FS_BASE    8*21
# define GS_BASE    8*22
# define DS	        8*23
# define ES	        8*24
# define FS	        8*25
# define GS	        8*26