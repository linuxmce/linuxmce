ROOT=..
CXXFLAGS = -fPIC -Wall -ggdb3 -g -D_GNU_SOURCE
CPPFLAGS = -I../../ -I../ -I$(ROOT) -I$(ROOT)/DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DBTSOCKET
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


sources = RA_Processor.cpp RA_Action.cpp RA_Request.cpp RAServer.cpp RAClientSocket.cpp RAServerSocket.cpp

all: libRACommon.so all-recursive

libRACommon.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib 

libRACommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libRACommon.so >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p pluto/lib
	cp libRACommon.so $(DESTPATH)pluto/lib
