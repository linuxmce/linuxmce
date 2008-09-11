ROOT=..

CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3 -DENABLE_TEST_UNIT
CXXFLAGS=-I. -I.. -I$(ROOT)
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lid3

sources = id3info.cpp \

all: $(sources:.cpp=.o)
	$(CXX) -o id3info $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f id3info $(sources:.cpp=.o) 
