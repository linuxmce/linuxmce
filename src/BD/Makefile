ROOT=..
CXXFLAGS = -fpermissive -fPIC -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -I/usr/include/mysql -I$(ROOT) -I$(ROOT)/DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DXBTSOCKET -DBLUETOOTH_DONGLE -DMULTI_THREADED
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lmysqlclient_r

sources = BDCommand.cpp BDCommandProcessor.cpp BDCommandProcessor_Linux_Bluetooth.cpp sdpbrowse.cpp
#sources = BDCommand.cpp BDCommandProcessor.cpp

headers = *.h

all: libBDCommon.so all-recursive

libBDCommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

libBDCommon.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../lib
	cp $@ ../lib

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libBDCommon.{a,so} >/dev/null

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include
	cp libBDCommon.so $(DESTDIR)/usr/pluto/lib
	cp $(headers) $(DESTDIR)/usr/include
