ROOT = ../..
CPPFLAGS=-g -W -Wall
CXXFLAGS=-fPIC
LDFLAGS=
LDLIBS=-lconfuse

sources=$(wildcard *.cpp)

all: libresolution.so test

libresolution.so: libresolution.o
	g++ -shared $(CPPFLAGS) $(CXXFLAGS) $< -o libresolution.so $(LDFLAGS) $(LDLIBS)
	cp libresolution.so $(ROOT)/lib

test: test.o libresolution.so
	g++ $(CPPFLAGS) $(CXXFLAGS) $< -o test -L. -lresolution

clean:
	rm -f test libresolution.so *.o

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p pluto/lib
	cp libresolution.so $(DESTPATH)pluto/lib
