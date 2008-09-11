ROOT=../..

CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lPlutoUtils -lpthread -lmysqlclient_r

sources = main.cpp file_utils.cpp

all: $(sources:.cpp=.o)
	$(CXX) -o plutodevice $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f plutodevice $(sources:.cpp=.o) 
