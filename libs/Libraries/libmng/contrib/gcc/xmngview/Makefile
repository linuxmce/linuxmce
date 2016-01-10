CFLAGS=-Wall -O2 -march=pentium -mcpu=pentium
LIBS = -L/usr/X11R6/lib -L/usr/local/lib -lXm -lXt -lX11 -lmng -lm
CC=gcc
LDFLAGS=
PRG=xmngview

compile:
	$(CC) $(CFLAGS) color.c xmngplay.c -o $(PRG) $(LIBS)

clean:
	rm -f $(PRG)

install:
	cp -a $(PRG)  /usr/local/bin/$(PRG)
