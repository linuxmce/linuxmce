SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT=..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS ?= -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE

have_httpfetcher = $(wildcard /usr/lib/libhttp_fetcher.*)

ifneq ($(strip $(have_httpfetcher)),)
	CXXFLAGS += -DUSE_HTTP_FETCHER
endif

CXX = g++
CXXFLAGS += -fPIC
CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUSE_LZO_DATAGRID $(SNR_CPPFLAGS)
LDFLAGS += $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS += -lDCECommon

ifneq ($(strip $(have_httpfetcher)),)
	LDLIBS += -lhttp_fetcher
endif

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

sources = CommonIncludes.cpp FileUtils.cpp MultiThreadIncludes.cpp Other.cpp ProcessUtils.cpp StringUtils.cpp DatabaseUtils.cpp getch.cpp md5c.cpp minilzo.cpp uuencode.cpp LinuxSoundcard.cpp LinuxSerialUSB.cpp ThreadedClass.cpp \
	  ../db_wrapper/db_wrapper.cpp ../db_wrapper/ClientSocket.cpp ../db_wrapper/Socket.cpp

all: libPlutoUtils.so all-recursive

libPlutoUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../lib

libPlutoUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

distclean: clean

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libPlutoUtils.so libPlutoUtils.a >/dev/null

docs:
	doxygen Doxyfile

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p pluto/lib
	cp libPlutoUtils.so $(DESTPATH)pluto/lib
