SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fpermissive -fPIC -Wall -ggdb3
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE $(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -D_GNU_SOURCE -DLINK_TO_ROUTER $(EXTRA)
CXX = g++
LDFLAGS =
LDFLAGS += -L$(ROOT)/lib
LDLIBS = -lpthread -lSerializeClass

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

APPNAME = DCECommon
VERSION = 2.0.0

sources = DataGrid.cpp ClientSocket.cpp PlainClientSocket.cpp Logger.cpp Message.cpp \
	Socket.cpp ServerSocket.cpp SocketListener.cpp ServerLogger.cpp \
	AlarmManager.cpp Command_Impl.cpp Event_Impl.cpp \
	HandleRequestSocket.cpp \
	DeviceData_Router.cpp DeviceData_Impl.cpp DeviceData_Base.cpp \
	DCEConfig.cpp DCEMySqlConfig.cpp \
	Virtual_Device_Translator.cpp arp-cache.cpp \
	MessageBuffer.cpp \
	../db_wrapper/db_wrapper.cpp ../db_wrapper/ClientSocket.cpp ../db_wrapper/Socket.cpp
	
headers = *.h

all: libDCECommon.so all-recursive

libDCECommon.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../lib

libDCECommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

distclean: clean

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libDCECommon.so libDCECommon.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

docs: docs_api

docs_api:
	doxygen Doxyfile

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p pluto/lib
	mkdir -p $(DESTDIR)/usr/lib $(DESTDIR)/usr/include
	cp libDCECommon.so $(DESTPATH)pluto/lib
	cp libDCECommon.so $(DESTDIR)/usr/lib
	cp $(headers) $(DESTDIR)/usr/include	
