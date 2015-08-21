CC = gcc
CFLAGS = -Wall -fPIC -ggdb3

whattodo:
	@echo "type 'make clean', 'make bin' or 'make all'"

all: bin

bin: core-locator

core-locator: core-locator.c
	$(CC) -o core-locator core-locator.c $(CFLAGS)

clean: clean-recursive
	rm -f core-locator core-locator.o >/dev/null

install: 
	mkdir -p /usr/pluto/bin
	cp core-locator $(DESTPATH)/usr/pluto/bin
