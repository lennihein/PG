#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#define _SCHED_H 1
#define __USE_GNU 1
#include <bits/sched.h>
//#include <linux/sched.h>
//#include <sched.h>
#define STACK_SIZE 5096
#include <errno.h>
#include <stdlib.h>
//#define _GNU_SOURCE
/*       #include <linux/sched.h>

       int clone(int (*fn)(void *), void *child_stack,
                 int flags, void *arg, ...
                 /* pid_t *ptid, void *newtls, pid_t *ctid *///);


int payload(void *arg)//Payload funktion- Diese kann vom zukünftigen Jugaad 2.0 benutzter beliebig modifiziert werden.
{	
	
	printf("Jugaad was here!.\n");
	sleep(2);
	exit(0);
	//return 0;
}//Die rückgabe dieser Funktion bestimmt den exit Status des Clones.Den Child kann man auch terminieren in dem man exit called 



void shellcode(int (*fn)(void *)){

	
	int *ns_pointer = mmap(NULL,20000,PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	clone(fn,ns_pointer+STACK_SIZE,CLONE_THREAD|CLONE_VM|CLONE_SIGHAND,NULL);
	
}


int main()
{

	shellcode(payload);
	printf("parent is here\n");
	sleep(4);
	__asm__("int3");
	return 0;
}