.DEFAULT_GOAL := run
LIBS := -lzmq -lczmq
CFLAGS := -g -W -Werror

dd.o: dd.cpp header.hpp
	g++ -c -o dd.o dd.cpp $(CFLAGS) $(LIBS)

lib.o: lib.cpp header.hpp
	g++ -c -o lib.o lib.cpp $(CFLAGS) $(LIBS)

clean:
	rm *.o dd

compile: dd.o lib.o
	g++ -o dd dd.o lib.o $(CFLAGS) $(LIBS)

run: | compile
	@./$(DIR)dd
	@rm *.o dd