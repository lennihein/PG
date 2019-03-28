compile gcc -m64 -o jugaad2  jugaad2.c
compile gcc -m64 -o testproc testproc.c

open 2 Terminals 

1st Terminal:  ./testproc
2nd Terminal: sudo ./jugaad2  <pid>

optional
3rd Terminal:Watch mapping of 1st Terminal with  /proc/<pid>/maps 
compare before  and after ./jugaad2 when using a shellcode that inlcudes sys_mmap

Using other shellcodes need changes of "var" in jugaad2 and recompiling;
3Shellcodes are in Jugaad2
1:only executing a shell(no mmap/no clone)
2.:mmap&clone&Hallo_World as Payload
3:only sys_mmap 
