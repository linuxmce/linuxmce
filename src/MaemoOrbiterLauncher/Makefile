all: maemo11

maemo21:
	gcc -DMAEMO2 -ggdb3 $(shell pkg-config gdk-2.0 gtk+-2.0 hildon-lgpl --libs --cflags) -ggdb3 OrbiterLauncher.c -o OrbiterLauncher
maemo11:
	gcc $(shell pkg-config gtk+-2.0 hildon-lgpl --libs --cflags) OrbiterLauncher.c -o OrbiterLauncher
clean:
	rm -f OrbiterLauncher
