.DEFAULT_GOAL := compile_all

LIBS :=
DIR := build/
CFLAGS := -g -W
TARGETS := mdb mview
DEPS := $(wildcard *.h)
# OBJECTS :=$(patsubst %.c,$(DIR)%.o,$(wildcard *.c))

dir_exists: | $(DIR)

$(DIR)%.o: %.c
	@gcc -c -o $@ $^ $(CFLAGS) $(LIBS)

# todo: OBJECTS PATSUBST
compile_mdb: $(DIR)mdb.o $(DIR)functions.o $(DIR)assertion.o
	@gcc -o $(DIR)mdb $^ $(CFLAGS) $(LIBS)

compile_mview: $(DIR)mview.o $(DIR)assertion.o
	@gcc -o $(DIR)mview $^ $(CFLAGS) $(LIBS)

clean: | compile_all
	@rm -r -f $(DIR)

mdb: | compile_mdb
	@./$(DIR)mdb
	
mview: | compile_mview
	@./$(DIR)mview

v_mdb: | compile_mdb
	$(shell valgrind ./$(DIR)mdb)

v_mview: | compile_mview
	$(shell valgrind ./$(DIR)mview)

c: | compile_all

compile_all: | dir_exists compile_mdb compile_mview

disasm:
	@objdump -S -C -d -M intel $(DIR)mdb.o
	@echo 
	@objdump -S -C -d -M intel $(DIR)mview.o

$(DIR):
	@mkdir -p $@
