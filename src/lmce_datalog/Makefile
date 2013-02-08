ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib

sources = Database_lmce_datalog.cpp TableRow.cpp \
Table_Datapoints.cpp \
Table_Unit.cpp

headers = *.h

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

all: liblmce_datalog.so all-recursive

liblmce_datalog.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

distclean: clean

clean: clean-recursive
	rm -f *.o *.d liblmce_datalog.so >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include
	cp $(headers) $(DESTDIR)/usr/include
	cp liblmce_datalog.so $(DESTDIR)/usr/pluto/lib
