CC:= g++
CFLAGS:= -Wall -Wextra -pedantic -g
LIBS:= -lsfml-system -lsfml-window -lsfml-graphics

zamiv: zamiv.o config.hpp cxxopts.hpp
	$(CC) $(CFLAGS) $(LIBS) -o $@ zamiv.o

zamiv.o: zamiv.cpp config.hpp
	$(CC) $(CFLAGS) -c zamiv.cpp

config.hpp: config.def.hpp
	cp -n $? $@

clean:
	rm zamiv *.o
