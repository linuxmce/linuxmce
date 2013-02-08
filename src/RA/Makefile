ROOT=..
CXXFLAGS = -fPIC -Wall -ggdb3 -g -D_GNU_SOURCE
CPPFLAGS = -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DBTSOCKET
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

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
