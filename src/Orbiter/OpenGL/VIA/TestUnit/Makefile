
CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3
CPPFLAGS=-I. -I.. -I/usr/include/SDL -I/usr/X11R6/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -DKDE_LMCE -DDEBUG -DVIA_DIAG_TEST_UNIT
LDFLAGS=-lXtst -lGL -lGLU -lXrender -lXext -lSDL -lSDL_ttf -lSDL_image -lddmpeg -lglut 

sources = VIAdiag.cpp ../ViaOverlay.cpp

all: $(sources:.cpp=.o)
	$(CXX) -o VIAdiag $+ $(LDFLAGS)
	cp VIAdiag ../../../../bin/
clean:
	rm -f VIAdiag $(sources:.cpp=.o) 
