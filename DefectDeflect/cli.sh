nasm -f elf64 test_tracee.asm && ld -o tracee test_tracee.o && rm test_tracee.o && gcc -o cli cli.c -lzmq -lczmq -Werror && make compile && ./cli && rm cli && rm tracee && make clean