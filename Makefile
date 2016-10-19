CC=g++
CFLAGS=-Wall -Wextra -pedantic -O2 -Iinclude -std=c++11
tCFILES=$(wildcard src/*.cpp)
CFILES=$(tCFILES:src/%=%)
OFILES=$(CFILES:%.cpp=obj/%.o)
EXEC=Bomberman

LIB=-lsfml-graphics -lsfml-window -lsfml-system

ifdef DEBUG
CFLAGS=-Wall -Wextra -pedantic -g -Iinclude -std=c++11
LIB=-lsfml-graphics -lsfml-window -lsfml-system
endif

.PHONY all:
.PHONY clean:
.PHONY cleanall:
.PHONY run:

all: bin/$(EXEC)

bin/$(EXEC): $(OFILES)
	mkdir -p bin
	$(CC) -o $@ $(CFLAGS) $(OFILES) $(LIB)

obj/%.o: src/%.cpp
	mkdir -p obj
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f obj/*

cleanall:
	rm -f obj/* bin/$(EXEC)

run: bin/$(EXEC)
	bin/$(EXEC)

run_gdb: bin/$(EXEC)
	gdb bin/$(EXEC)

run_valgrind: bin/$(EXEC)
	valgrind --leak-check=full bin/$(EXEC)
