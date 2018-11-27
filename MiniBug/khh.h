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
#include <sys/un.h>

#include <unistd.h>

// misc
#include <string.h>

// defines
#define __PORT__ 4711
#define __SOCKET_PATH__ "tmp_sock"
#define __MSG_SIZE__ 64