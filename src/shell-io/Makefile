CC=gcc
CFLAGS=--std=gnu99 -pipe -W -Wall -ggdb3

Binaries=fdread fdwrite fdselect

all: build

build: $(Binaries)

fdread: fdread.c fdio.c fdio.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $+ -o $@ $(LDFLAGS) $(LDLIBS)

fdwrite: fdwrite.c fdio.c fdio.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $+ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(Binaries)

install: build
	mkdir -p $(DESTDIR)/usr/pluto/bin/
	cp $(Binaries) $(DESTDIR)/usr/pluto/bin/

.PHONY: all build clean install
