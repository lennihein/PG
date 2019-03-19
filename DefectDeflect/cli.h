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

#define RAX "80"