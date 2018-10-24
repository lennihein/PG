<table>
  <tr>
    <th><h2> <a href="https://pg.bwuah.me/prep-task"> TASK </a> </h2></th>
    <th> <h2> <a href="https://pg.bwuah.me/prep"> SOLUTION </a> </h2></th>
  </tr>
</table>

---

## 1

### a)

```C
#include <stdio.h>
#include <stdlib.h>

int main()
{   
    unsigned long rip;
    asm("leaq (%%rip), %0;": "=r"(rip));
    printf("%lx\n", rip);
    return EXIT_SUCCESS;
}
```

### b)

- `int (* call_shell)()` ist eine Deklaration einer Funktion ohne Eingabe mit Ausgabe `int`.
- `int(*)()` ist der Datentyp einer Funktion ohne Eingabe mit Ausgabe `int`.
- `shellcode` ist ein Bitmuster.

Die gesamte Main führt also die Funktion aus, die durch das Bitmuster, welches als String übergeben wird, definiert wird.

### c)

```C
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signo){//Signal handling funktion erhält ein bestimmtes signals
	if(signo == SIGINT){
		printf("received SIGINT\n");
	}
	if(signo == SIGKILL){
		printf("received SIGKILL\n");
	}
	if(signo == SIGSTOP){
		printf("received SIGSTOP\n");
	}
}

int main(){
	if(signal(SIGINT, signal_handler)== SIG_ERR){//signal handler funktion wird im kernel registriert
	printf("\nsignal error\n");
	}
	if(signal(SIGKILL, signal_handler)== SIG_ERR){
	printf("\nsignal error\n");
	}
	if(signal(SIGSTOP, signal_handler)== SIG_ERR){
	printf("\nsignal error\n");
	}
	while(1){
		sleep(1);
	}
	return 0;	
}
```

oder

```C
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int sig);

int main()
{
    printf("PID : %d\n", getpid());
    if(signal(SIGINT, sig_handler) == SIG_ERR) exit(1);
    int x = 0;
    loop:
    sleep(3);
    printf("Hey(%i)\n", x++);
    goto loop;
    return EXIT_SUCCESS;
}

void sig_handler(int sig)
{
    printf("Caught Signal\n");
    exit(EXIT_SUCCESS);
}
```

### d)

```asm
section .data

    str: db "Hello World", 10
    str_len equ $ - str         
    ; current adress - str label adress = length of "Hello World\n"
    
section .text
global _start

_start:

    ; sys_write
    mov RAX, 1
    mov RDI, 1          ; stdout
    mov RSI, str        
    mov RDX, str_len    
    syscall

    ; sys_exit
    mov RAX, 60
    mov RDI, 0          ; error code
    syscall

```

---

## 2

- Was ist ein Prozess? <br>
-> 
- Was ist ein Thread? <br>
->Ein Thread ist ein Teil eines Prozesses. Threads zum selben Prozess teilen sich den Codesegment und Datasegment.
   Mithilfe von Mutex, lassen sich Speicherzugriffe verwalten, damit die Threads nicht miteinander in Konflikte geraten.
- Wie werden Prozesse in Linux identifiziert?  <br>
->
- Was ist der Zweck von clone, fork, execve?  <br>
->
- Was ist ptrace? <br>
->
