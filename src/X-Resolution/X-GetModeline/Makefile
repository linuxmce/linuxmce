ROOT = ../..
CPPFLAGS=-g -W -Wall
CXXFLAGS=-fPIC
LDFLAGS=-L../libresolution
LDLIBS=-lresolution

sources=$(wildcard *.cpp)

all: $(ROOT)/bin/X-GetModeline

X-GetModeline: X-GetModeline.o ../libresolution/libresolution.so
	g++ $(CPPFLAGS) $(CXXFLAGS) $< -o X-GetModeline $(LDFLAGS) $(LDLIBS)

$(ROOT)/bin/X-GetModeline: X-GetModeline
	cp X-GetModeline $@

clean:
	rm -f X-GetModeline $(ROOT)/bin/X-GetModeline *.o

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
