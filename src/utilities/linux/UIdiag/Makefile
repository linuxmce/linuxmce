CXX=g++
CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3
CPPFLAGS=-I. -I/usr/include/SDL -I/usr/X11R6/include $(shell pkg-config --cflags glib-2.0)
LDFLAGS=-lXtst -lGL -lGLU -lXrender -lXext -lSDL -lSDL_ttf -lSDL_image $(shell pkg-config --libs glib-2.0)

sources = UIdiag.c ../CompositeHelper.cpp
			
all: $(sources:.c=.o)
	$(CXX) -o UIdiag $+ $(LDFLAGS)
	cp UIdiag ../../../bin/
clean:
	rm -f UIdiag UIdiag.o ../CompositeHelper.o 
	

install: 
	mkdir -p /etc
	cp Orbiter.conf $(DESTPATH)/etc
	mkdir -p /usr/pluto/bin
	cp Orbiter $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sis $(DESTPATH)/usr/pluto/bin
	cp LaunchOrbiter.sh $(DESTPATH)/usr/pluto/bin
	cp OrbiterGL $(DESTPATH)/usr/pluto/bin
	cp UIdiag $(DESTPATH)/usr/pluto/bin
	cp logo.png $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sisx $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sisx $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/share
	cp GTKOrbiter.glade $(DESTPATH)pluto/share
	cp orbiter-gtkrc $(DESTPATH)pluto/share
