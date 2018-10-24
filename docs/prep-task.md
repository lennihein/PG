<table>
  <tr>
    <th><h2> <a href="https://pg.bwuah.me/prep-task"> TASK </a> </h2></th>
    <th> <h2> <a href="https://pg.bwuah.me/prep"> SOLUTION </a> </h2></th>
  </tr>
</table>

---

| [TASK](https://pg.bwuah.me/prep-task) | [SOLUTION](https://pg.bwuah.me/prep) |
|:---:|:---:|

---

**1)** Erinnert euch nochmal an eure C und ASM Kenntnisse und erweitert
diese. Insbesondere sollt ihr umgehen können (d.h. Code lesen und
produzieren) mit:
C:
- Structs
- Funktionspointer
- ASM in C ausführen
- Signalhandler
ASM:
- Auf dem Papier richtig ASM Programmieren können
- Linux Callingconventions (Achtung: Unterschied bei x86 und x86-64. Für
x64 siehe Wikipedia oder [link](https://software.intel.com/sites/default/files/article/402129/mpx-linux64-abi.pdf))
- Systemcalls
- Linux Systemcalls ([link](http://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/))


Um zu euch zu testen, versucht mal (im Team) folgende Aufgaben zu lösen.
Dabei geht es nicht darum "die perfekte Abgabe" zu produzieren sondern
ihr sollt euch klar machen, was ihr schon versteht und wo ihr noch
Klärnugsbedarf habt.

a) Lest mal in einem C-Programm den RIP aus. (Erste Schritte [hier](https://en.wikipedia.org/wiki/Inline_assembler))

b) Könnt ihr erklären, was in der Main passiert? <br>

```C
unsigned char shellcode[] = "...";
void main() 
{
    int (*call_shell)() = (int(*)())shellcode;
    call_shell();
}
```

c) Eigenen Signalhandler schreiben und testen (unter Linux sendet ihr
Signals mit dem befehl kill. Siehe man kill und kill -l)

d) Schreibt und erklärt ein ASM-"Hello World". (Wenn ihr nicht weiterkommt: [link](http://0xax.blogspot.com/2014/08/say-hello-to-x64-assembly-part-1.html))


**2)** Wiederholt / Lernt:<br>
Was ist ein Prozess? <br>
Was ist ein Thread?<br>
Wie werden Prozesse in Linux identifiziert? (Was ist eine PID?)<br>
Was ist der Zweck von clone, fork, execve? Was sind Unterschiede? Was
sind Gemeinsamkeiten?<br>
Macht euch mit ptrace vertraut. (Beginnt mit dem Wikipediaeintrag. Es
geht nicht darum die Details zu verstehen. Es soll zunächst nur klar
werden, was man mit ptrace machen könnte.)

Ziel bis zu unserem Treffen ist es die folgenden beiden Artikel zu
verstehen. <br>
  [Part1](https://www.linuxjournal.com/article/6100) <br>
  [Part2](https://www.linuxjournal.com/article/6210) <br>

