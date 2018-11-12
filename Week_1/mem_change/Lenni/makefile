.DEFAULT_GOAL := all

LIBS :=
CC  := gcc
CFLAGS := -g -W -O0 -fno-stack-protector -z execstack

%.o: %.c
	gcc -c -o $@ $^

main: main.o
	gcc -o $@ $^ $(CFLAS) $(LIBS)

dummy: dummy.o
	gcc -o $@ $^ $(CFLAS) $(LIBS)

all:
	@echo $(shell make main)
	@echo $(shell make dummy)
	@echo $(shell make test)

clean: | all
	rm *.o main dummy a.out

disasm:
	@objdump -S -C -d -M intel main.o
	@echo 
	@objdump -S -C -d -M intel dummy.o
	@echo 
	@objdump -S -C -d -M intel test.o

test.o: test.asm
	nasm -f elf64 test.asm

test: test.o
	ld test.o
	
run:
	./main ./a.out