ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib

sources = Database_lmce_datalog.cpp TableRow.cpp \
Table_Datapoints.cpp \
Table_Unit.cpp

headers = *.h

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

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
