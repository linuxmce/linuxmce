ROOT = ../..
CPPFLAGS=-g -W -Wall
CXXFLAGS=-fPIC
LDFLAGS=
LDLIBS=-lconfuse

sources=$(wildcard *.cpp)

all: $(ROOT)/lib/libresolution.so test

libresolution.so: libresolution.o
	g++ -shared $(CPPFLAGS) $(CXXFLAGS) $< -o libresolution.so $(LDFLAGS) $(LDLIBS)

$(ROOT)/lib/libresolution.so: libresolution.so
	cp libresolution.so $@

test: test.o $(ROOT)/lib/libresolution.so
	g++ $(CPPFLAGS) $(CXXFLAGS) $< -o test -L. -lresolution

clean:
	rm -f test libresolution.so $(ROOT)/lib/libresolution.so *.o

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
