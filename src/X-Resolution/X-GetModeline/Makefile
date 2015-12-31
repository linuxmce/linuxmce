ROOT = ../..
CPPFLAGS=-g -W -Wall
CXXFLAGS=-fPIC
LDFLAGS=-L../libresolution
LDLIBS=-lresolution

sources=$(wildcard *.cpp)

all: X-GetModeline

X-GetModeline: X-GetModeline.o
	g++ $(CPPFLAGS) $(CXXFLAGS) $< -o X-GetModeline $(LDFLAGS) $(LDLIBS)
	cp X-GetModeline $(ROOT)/bin

clean:
	rm -f X-GetModeline *.o

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p /usr/pluto/bin
	cp *.sh $(DESTPATH)/usr/pluto/bin
	cp *.pl $(DESTPATH)/usr/pluto/bin
	cp *.awk $(DESTPATH)/usr/pluto/bin
	cp X-GetModeline $(DESTPATH)/usr/pluto/bin
	cp *.py $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/share
	cp *.png $(DESTPATH)pluto/share
	cp Resolutions.conf $(DESTPATH)pluto/share
	mkdir -p pluto/templates
	cp xorg.conf.in $(DESTPATH)pluto/templates
