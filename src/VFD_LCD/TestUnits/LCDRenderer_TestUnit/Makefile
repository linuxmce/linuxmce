ROOT=../../..

CXX=g++
CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3 -Werror
CPPFLAGS=-I. -I../../..
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib -lDCECommon -lPlutoUtils -lSerial

sources = ../../Renderers/LCDRenderer.cpp main.cpp
			
all: $(sources:.cpp=.o)
	$(CXX) -o LCDRenderer $+ $(LDFLAGS)
	cp LCDRenderer ../../../bin/
clean:
	rm -f LCDRenderer $(sources:.cpp=.o) 
	
