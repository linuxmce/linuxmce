DCE_PATH?=../..

CXX=g++
CPPFLAGS=-I. -I$(DCE_PATH) -pipe -W -Wall -ggdb3
LDFLAGS=-L$(DCE_PATH)/lib
LDLIBS=-lSerializeClass -lDCECommon -lPlutoUtils

CPPsources=Main.cpp Whisperer.cpp ShellInterface.cpp

all: DCE-Whisperer

DCE-Whisperer: $(CPPsources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f DCE-Whisperer *.o

include $(DCE_PATH)/PlutoUtils/make.rules
-include $(CPPsources:.cpp=.d)
