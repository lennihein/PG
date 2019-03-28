#define _SCHED_H 1
#define __USE_GNU 1
#include <bits/sched.h>

int main(){
	return CLONE_THREAD|CLONE_VM|CLONE_SIGHAND;
}
/*
output with gcc -E flags_clone.cabs

CLONE_THREAD-> 0x00010000
CLONE_VM->  0x00000100
CLONE_SIGHAND-> 0x00000800

*/