//test what defines
#include <sys/mman.h>

int main(){

	return MAP_PRIVATE | MAP_ANONYMOUS;
}

/*
Nach ausgabe von gcc -E flags.c

MAP_PRIVATE ->0x02

MAP_ANONYMOUS->0x20


*/
// int main(){

	// return PROT_EXEC | PROT_READ | PROT_WRITE;
// }
/*
PROT_EXEC->0x4
PROT_READ->0x1
PROT_WRITE->0x2
*/