CC=gcc
CPPFLAGS=-W -Wall -pipe #-g -ggdb3
CFLAGS=
LDFLAGS=
LDLIBS=

TARGETS=Maxtor_openmss_EnableNFS

all: $(TARGETS)
	cp $(TARGETS) ../bin

clean:
	rm -f $(TARGETS)

install: 
	mkdir -p /usr/pluto/bin
	cp *.sh $(DESTPATH)/usr/pluto/bin
	cp Maxtor_openmss_EnableNFS $(DESTPATH)/usr/pluto/bin
