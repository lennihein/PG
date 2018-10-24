.DEFAULT_GOAL := hello
.PHONY:	hello clean

hello: 
	@echo "run make [option] with option:\nrip\nsig\nasm\nclean"

rip: dump_rip.c
	@gcc dump_rip.c
	@./a.out

sig: sig.c
	@gcc sig.c
	@./a.out

asm: hello_world.o
	@ld hello_world.o
	@./a.out

hello_world.o: hello_world.asm
	@nasm -f elf64 hello_world.asm

clean:
	@rm *.o *.out