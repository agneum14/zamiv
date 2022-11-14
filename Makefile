CC:= g++
CFLAGS:= -Wall -Wextra -Werror -pedantic -g

zamiv: zamiv.o cxxopts.hpp
	$(CC) $(CFLAGS) -o $@ $?

zamiv.o: zamiv.cpp
	$(CC) $(CFLAGS) -c $?

clean:
	rm zamiv *.o
