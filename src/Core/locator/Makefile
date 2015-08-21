CC = gcc
CFLAGS = -Wall -fPIC -ggdb3

all: bin

bin: core-locator

core-locator: core-locator.c
	$(CC) -o core-locator core-locator.c $(CFLAGS)

clean:
	rm -f core-locator core-locator.o >/dev/null

install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin/
	cp -afv core-locator $(DESTDIR)/usr/pluto/bin/

