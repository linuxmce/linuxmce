ROOT=../..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lPlutoUtils -lpthread

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources = main.cpp file_utils.cpp
			
all: $(sources:.cpp=.o)
	$(CXX) -o plutodevice $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f plutodevice $(sources:.cpp=.o) 
	
