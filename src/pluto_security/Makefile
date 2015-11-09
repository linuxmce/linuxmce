ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib
sources = Database_pluto_security.cpp TableRow.cpp
#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

sources +=  \
Table_Alert.cpp \
Table_AlertType.cpp \
Table_Alert_Device.cpp \
Table_ModeChange.cpp \
Table_Notification.cpp \
Table_psc_security_batdet.cpp \
Table_psc_security_bathdr.cpp \
Table_psc_security_batuser.cpp \
Table_psc_security_repset.cpp \
Table_psc_security_schema.cpp \
Table_psc_security_tables.cpp

all: libpluto_security.so all-recursive

libpluto_security.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_security.so >/dev/null

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp $@ /usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
