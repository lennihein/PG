nasm -f elf64 test_tracee.asm
ld -o tracee test_tracee.o
rm test_tracee.o