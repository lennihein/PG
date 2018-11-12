.DEFAULT_GOAL := all

LIBS :=
CC  := gcc
CFLAGS := -g -W -O0 -fno-stack-protector -z execstack

%.o: %.c
	gcc -c -o $@ $^

tracer: tracer.o
	gcc -o $@ $^ $(CFLAS) $(LIBS)

dummy: dummy.o
	gcc -o $@ $^ $(CFLAS) $(LIBS)

all:
	@echo $(shell make tracer)
	@echo $(shell make dummy)

clean: | all
	rm *.o tracer dummy

disasm:
	@objdump -S -C -d -M intel tracer.o
	@echo 
	@objdump -S -C -d -M intel dummy.o