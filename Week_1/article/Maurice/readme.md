ptrace Artikel für 64 Bit

In ptrace_practice sind Methoden zum besseren Verständnis zu ptrace implementiert:

peekuser()
-parent unterbricht child
-über ptrace_peekuser liest parent befehl von child aus
-parent gibt syscall nummer aus
-child wird fortgeführt und führt Befehl aus

peekuserdata()
-selber output wie peekuser()
-zusätzlich werden andere Adressen ausgegeben

getregstest()
-output genau wie peekuserdata()
-benutzt aber struct zum speichern der Adressen