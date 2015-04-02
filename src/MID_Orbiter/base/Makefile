PROGRAM=interactor-disked

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++
CC=gcc

all: bin

bin: $(PROGRAM)
	cp $(PROGRAM) ../../bin/

interactor-disked:
	$(CXX) $(PROGRAM).c $(CPPFLAGS) -o interactor-disked $+
	
clean:
	rm -f $(PROGRAM) *.o
