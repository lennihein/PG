##Wichtige Signals die abgefangen werden sollten

###SIGILL,SIGFPE 
wenn ein programm eine illegale instruction versucht auszuführen

###SIGABRT 
kommt vom abort() call.(Können auch von libraries kommen)

###SIGSEV
invalid memory access

###SIGBUS
kommt vor bei mmap() und speicherbereichen mit read/write rechte fehlern

###SIGPIPE
I/0 on pipes and pipes get disconnceted from other side

###POSIX
spezifiziert SIGSYS bei invaliden SysCalls

##Bei Runtime Signals

###SIGALRAM
when using alarm()

###SIGPOLL
when using polling

###SIGCHLD
when spwaning a process

###SIGHUP
when terminal with programm dies

###SIGINT
for ^c key

###SIGQUIT
for ^\ key

###SIGTERM
für termination request

###SIGKILL
im Standart sollte es nicht abgefangen werden

##GDB handlet noch folgende Signals

##1
|Signal   | Stop | Print | Pass to programm | Description            |
|---------|------|-------|------------------|------------------------|
|SIGHUP   |  Yes |   Yes |              Yes |      Hangup            |
|---------|------|-------|------------------|------------------------|
|SIGINT   |  Yes |   Yes |              No  |   Interrupt            |
|---------|------|-------|------------------|------------------------|
|SIGQUIT  |  Yes |   Yes |              Yes |  Quit                  |
|---------|------|-------|------------------|------------------------|
|SIGILL   |  Yes |   Yes |              Yes | Illegal instruction    |
|---------|------|-------|------------------|------------------------|
|SIGQABRT |  Yes |   Yes |              NO  | Aborted                |
|---------|------|-------|------------------|------------------------|
|SIGEMT   |  Yes |   Yes |              Yes | Emulation Trap         |
|---------|------|-------|------------------|------------------------|
|SIGFPE   |  Yes |   Yes |              Yes | Arithmetic exceptoin   |
|---------|------|-------|------------------|------------------------|
|SIGKILL  |  Yes |   Yes |              Yes | Killed                 |
|---------|------|-------|------------------|------------------------|
|SIGBUS   |  Yes |   Yes |              Yes | Bus error              |
|---------|------|-------|------------------|------------------------|
|SIGQSEV  |  Yes |   Yes |              Yes | Segmentation fault     |
|---------|------|-------|------------------|------------------------|
|SIGSYS   |  Yes |   Yes |              Yes | Bad System Call        |
|---------|------|-------|------------------|------------------------|
|SIGPIPE  |  Yes |   Yes |              Yes | Broken pipe            |
|---------|------|-------|------------------|------------------------|
|SIGALRM  |  No  |   No  |              Yes | Alarm clock            |
|---------|------|-------|------------------|------------------------|
|SIGTERM  |  Yes |   Yes |              Yes | Terminated             |
|---------|------|-------|------------------|------------------------|
|SIGURG   |  No  |   No  |              Yes | Urgent I/0 condition   |
|---------|------|-------|------------------|------------------------|
|SIGSTOP  |  Yes |   Yes |              Yes | Stopped(signal)        |
|---------|------|-------|------------------|------------------------|
|SIGTSTP  |  Yes |   Yes |              Yes | Stopped (user)         |
|---------|------|-------|------------------|------------------------|
|SIGCONT  |  Yes |   Yes |              Yes | Continued              |
|---------|------|-------|------------------|------------------------|
|SIGCHLD  |  NO  |   NO  |              Yes | Child status changed   |
|---------|------|-------|------------------|------------------------|
|SIGTTIN  |  Yes |   Yes |              Yes | Stopped(tty input)     |
|---------|------|-------|------------------|------------------------|
|SIGTTOUT |  Yes |   Yes |              Yes | Stopped(tty output)    |
|---------|------|-------|------------------|------------------------|
|SIGIO    |  NO  |   NO  |              Yes | I/0 possible           |
|---------|------|-------|------------------|------------------------|
|SIGCPU   |  Yes |   Yes |              Yes | Cpu time limit exceed  |
|---------|------|-------|------------------|------------------------|
|SIGFSZ   |  Yes |   Yes |              Yes | File size limit exceed |
|---------|------|-------|------------------|------------------------|
|SIGVTALRM|  NO  |   No  |              Yes | Virtual timer expired  |
|---------|------|-------|------------------|------------------------|
|SIGPROF  |  No  |   No  |              Yes | Profiling timer expired|
|---------|------|-------|------------------|------------------------|
|SIGWINCH |  NO  |   No  |              Yes | Window size changed    |