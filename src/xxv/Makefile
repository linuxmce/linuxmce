ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

sources = ../db_wrapper/db_wrapper.cpp ../db_wrapper/ClientSocket.cpp ../db_wrapper/Socket.cpp Database_xxv.cpp TableRow.cpp \
Table_AUTOTIMER.cpp \
Table_CHANNELGROUPS.cpp \
Table_CHANNELS.cpp \
Table_CHRONICLE.cpp \
Table_EPG.cpp \
Table_MUSIC.cpp \
Table_OLDEPG.cpp \
Table_RECORDS.cpp \
Table_TIMERS.cpp \
Table_USER.cpp \
Table_VERSION.cpp

all: libxxv.so all-recursive

libxxv.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libxxv.so >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
