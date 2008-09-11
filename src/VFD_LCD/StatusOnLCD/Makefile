ROOT=../..

CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3 -Werror
CPPFLAGS=-I. -I../..
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib -lDCECommon -lPlutoUtils

sources = main.cpp

all: $(sources:.cpp=.o)
	$(CXX) -o StatusOnLCD $+ $(LDFLAGS)
	cp StatusOnLCD ../../bin/
clean:
	rm -f StatusOnLCD $(sources:.cpp=.o) 

