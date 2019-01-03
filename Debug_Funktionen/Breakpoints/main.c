#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>


const int long_size=sizeof(long);
// pid -> child , addr ->aktuelles rip, *str ->pointer wohin gespeichert werden soll, len -> länge bis wohin gelesen und gespeichert wird
void getdata(pid_t pid, long addr, char *str, int len)
{
	char *laddr;
	int i ,j;
	union u
	{
		long val;
		char chars[long_size];
	}data;//speicher für gelesene datein unter linux eine union datei
		
	i=0;
	j=len/long_size; //const wie viel gelesen werden soll bzw wie häufig wiederholt werden muss
	laddr = str;
		
	while(i<j)
	{
		data.val= ptrace(PTRACE_PEEKDATA, pid, addr +i*8, NULL);//  8 bytes auslesen und in data.val abspeichern
		memcpy(laddr, data.chars, long_size); // copieren von zwischenspeicher nach pointer laddr
		++i;
		laddr += long_size; //pointer von laddr weiter verschieben-> weil ziel ort im speicher zusammenhängend
	}
	j=len % long_size;// falls weniger als 1byte übertragen werden muss
		
	if(j!=0)
	{
		data.val= ptrace(PTRACE_PEEKDATA, pid, addr +i*8, NULL);
		memcpy(laddr, data.chars, j);
	}
	str[len] = '\0'; //?
}

void putdata(pid_t pid, long addr, char *str, int len)
{
	char *laddr;
	int i ,j;
	union u
	{
		long val;
		char chars[long_size];
	}data;//speicher von gelesene datein
				
	i=0;
	j=len/long_size;
	laddr = str;
	while(i<j)
	{
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, pid, addr +i*8, data.val);
		++i;
		laddr += long_size;
	}
	j=len%long_size;
	if(j!=0)
	{
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, pid, addr +i*8, data.val);
	}	
}




int main(int argc, char * argv[])
{
	pid_t child;
	struct user_regs_struct regs,regs2;
	int status;
	//int status2 =0;
	
	char code[]={
		0xcd,0x80,	//int 80
		0xcc	 	//int3
	};
	char backup[8];
	child=fork();
	if(child==0)
	{
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL))
		{
			perror("ptrace");
			return 1;
		}
		execvp(argv[1], argv+1);
		
	}
	else{
		wait(NULL);
		ptrace(PTRACE_GETREGS, child, NULL, &regs);
		
								ptrace(PTRACE_GETREGS, child, NULL, &regs2);
								fprintf(stderr, "RIP= %llx\n", regs2.rip);
								fprintf(stderr, "RAX = %lld,\nRDI= %lld,\nRSI= %lld,\nRDX= %lld\n\n",regs2.rax,regs2.rdi,regs2.rsi,regs2.rdx);
								
								
								ptrace(PTRACE_GETREGS, child, NULL, &regs);
								fprintf(stderr, "RIP= %llx\n", regs.rip);
								fprintf(stderr, "RAX = %lld,\nRDI= %lld,\nRSI= %lld,\nRDX= %lld\n\n",regs.rax,regs.rdi,regs.rsi,regs.rdx);
		//data bereich holen
		getdata(child, regs.rip, backup, 8); // hier bei der 2. eingabe ist die eingabe einer beliebigen addresse möglich !
		// auch wichtig ist das die abglesenen bytes 8 sind wegen 64bit ! und nicht nur die länge des später hinzugefügten bereichs!!
		//trap funktion --> break einfügen
		putdata(child, regs.rip, code, 3);
		
		ptrace(PTRACE_CONT, child, NULL, NULL);//child soll trap funktion ausführen
		
		//Trapfunktion ausgeführt parent erhällt kontrolle
		wait(NULL);
								ptrace(PTRACE_GETREGS, child, NULL, &regs2);
								fprintf(stderr, "RIP= %llx\n", regs2.rip);
								fprintf(stderr, "RAX = %lld,\nRDI= %lld,\nRSI= %lld,\nRDX= %lld\n\n",regs2.rax,regs2.rdi,regs2.rsi,regs2.rdx);
		printf("Breakpoint wurde erreicht, zum fortfahren [beliebige Taste] druecken");
		
		getchar(); 
		
		//trapfunktion mit backup wieder überschreiben
		putdata(child, regs.rip, backup, 8);
		ptrace(PTRACE_SETREGS, child, NULL, &regs);
		
		ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);// evt auch ptrace_cont -- hier nur singlestep zum überprüfen der ausgabe
		loop:
			waitpid(child,&status,0);
								ptrace(PTRACE_GETREGS, child, NULL, &regs2);
								fprintf(stderr, "RIP= %llx\n", regs2.rip);
								fprintf(stderr, "RAX = %lld,\nRDI= %lld,\nRSI= %lld,\nRDX= %lld\n\n",regs2.rax,regs2.rdi,regs2.rsi,regs2.rdx);
			if(WIFEXITED(status))
			{
				return 0;
			}
			ptrace(PTRACE_SINGLESTEP, child, 0,0);

		goto loop;
	}
}