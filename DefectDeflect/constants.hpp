#define ENDPOINT "tcp://127.0.0.1:5555"

#define __RETURN__ 1
#define __EXIT__ -1

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

// todo: define more syscalls
#define SYS_WRITE   1