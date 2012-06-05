ROOT = ..
CXXFLAGS += -Wall -ggdb3
CPPFLAGS += -I.. -I. -I/usr/include/mysql
LDFLAGS += -L$(ROOT)/lib
LDLIBS += -lmysqlclient -lpthread 

APPNAME = mysql_wrapper
VERSION = 2.0.0

sources = Main.cpp ServerSocket.cpp Socket.cpp

all: mysql_wrapper

mysql_wrapper: $(sources:.cpp=.o) Makefile
	$(CXX) -o $@ $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) mysql_wrapper >/dev/null

include $(ROOT)/PlutoUtils/make.rules


install: 
	mkdir -p /usr/pluto/bin
	cp mysql_wrapper $(DESTPATH)/usr/pluto/bin
