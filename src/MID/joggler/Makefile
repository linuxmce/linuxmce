PROGRAMS=ambient-dimmer

CPPFLAGS=-g -ggdb3 -W -Wall
CXX=g++
CC=gcc

all: $(PROGRAMS)
	cp $(PROGRAMS) ../../bin/

ambient-dimmer: ambient-dimmer.o 
	$(CXX) $(CPPFLAGS) -o ambient-dimmer $+

clean:
	rm -f $(PROGRAMS) *.o

