CC=gcc
CFLAGS=-W -Wall -g -ggdb3

all: ansi2html
	cp ansi2html ../bin

clean:
	rm -f ansi2html

install: 
	mkdir -p /usr/pluto/bin
	cp ansi2html $(DESTPATH)/usr/pluto/bin
