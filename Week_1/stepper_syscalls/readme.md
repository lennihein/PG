## Eine Art Stepper.

**Vor** und **nach** jedem Syscall wird der Tracee ge-trapt und der Tracer erlangt Kontrolle.
Wichtig ist der Toggle, damit zwischen `@before` und `@after` unterschieden werden kann.  
Wollen wir den `RAX` auslesen, dann sollten wir `orig_rax` verwenden, `rax` ist lediglich nützlich, wenn wir im `@after` Teil den Rückgabewert (im RAX) des Syscalls auslesen wollen.

Struktur:

```C
// warten auf erneuten interrupt und ggf. (sys_exit) handling von Beendigung des tracees.
waitpid(pid, &status, 0);   
if(WIFEXITED(status)) { handle(); }

// before-after toggle
if(toggle)
{
    toggle = 0;

    @after
}
else
{
    toggle = 1;

    @before
} 

// ge-trap-ten Callee wieder fortsetzen, dabei neuen Breakpoint am nächsten Syscall Interrupt setzen.
ptrace(PTRACE_SYSCALL, pid, 0,0);
```