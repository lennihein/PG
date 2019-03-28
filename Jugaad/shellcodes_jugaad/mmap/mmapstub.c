#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

// void shellcode(){
	// mmap(0,20000,PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,0);
	// /*
	// if (region == MAP_FAILED){
		// printf("Error nr %d\n",errno);
		// exit(EXIT_FAILURE);
	// }
	// */
// }

//compile mit gcc -m64 mmapstub -O0 -o mmapcode       *wichtig Optimazation aus*
//vor dem compilieren möglichekeit stack buffer checks zu deaktivieren ! Genauso wie -static für genauern opjdump
//-fno-stack-protector  -fno-stack-protector-all

//betrachten des opcodes vom c code kann bei der erstellung von den shellcodes später helfen
int main(){
	
	//shellcode();
	mmap(0,20000,PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,0);
	return 0;
}
