#include <stdint.h>

#define __MSG_SIZE__            0x40
#define __REQ_SIZE__            0x10

typedef struct MESSAGE
{
    char str[64];
}MESSAGE;

typedef struct REQUEST
{
    uint64_t control;
    uint64_t value;
}REQUEST;

// careful with stack alignment 
// https://stackoverflow.com/a/5435890/8114293 
// #define __REQ_SIZE__ sizeof(struct REQUEST)

// REQUESTS

// control

#define     __EOF__             0x00
#define     __REQ_REP__         0x01
#define     __CMD__             0x02
#define     __RECEIVED__        0x03
#define     __DATA_SIZE__       0x04
#define     __VALUE__           0x05
#define     __UNRECOGN__        0x06   

// value

#define     __ANY__             0x00
#define     __EXIT__            0x01

#define     __PEEK_REG__        0x11

#define     __NEXT_SYSCALL__    0x21    // TODO
