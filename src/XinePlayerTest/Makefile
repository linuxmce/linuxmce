CXX = g++
CXXFLAGS = -Wall -ggdb3
LDFLAGS += -L/usr/X11R6/lib
LDLIBS = -lxine -lX11 -lXext

MYNAME = XinePlayer

sources = Main.cpp XinePlayer.cpp XineStream.cpp XineOsdObject.cpp

all: bin

bin: XinePlayer

XinePlayer: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDLIBS) $(LDFLAGS)

clean:
	rm -f $(sources:.cpp=.o) $(MYNAME)

