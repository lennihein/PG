.DEFAULT_GOAL := all

LIBS :=
CC  := gcc
CFLAGS := -g -W -O0 -fno-stack-protector -z execstack

hello_from_c.o: hello.c
	gcc -c -o $@ $^

hello_from_c: hello_from_c.o
	gcc -o $@ $^ $(CFLAS) $(LIBS)

all:
	@echo $(shell make hello_from_c)
	@echo $(shell make hello_from_asm)

clean: | all
	rm *.o hello_from_c hello_from_asm

disasm:
	@objdump -S -C -d -M intel hello_from_c.o
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@objdump -S -C -d -M intel hello_from_asm.o

hello_from_asm.o: hello.asm
	nasm -f elf64 hello.asm
	mv hello.o hello_from_asm.o

hello_from_asm: hello_from_asm.o
	ld hello_from_asm.o
	mv ./a.out hello_from_asm