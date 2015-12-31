PROGRAMS=interactor-disked

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++
CC=gcc

all: $(PROGRAMS)
	cp $(PROGRAMS) ../../bin/

interactor-disked: interactor-disked.o 
	$(CXX) $(CPPFLAGS) -o interactor-disked $+

clean:
	rm -f $(PROGRAMS) *.o

