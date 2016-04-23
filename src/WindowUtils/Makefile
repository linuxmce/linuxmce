SNR_CPPFLAGS ?= #-DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine1-pluto/include -I/opt/linphone-1.3.5/include

ROOT=..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE

have_httpfetcher = $(wildcard /usr/lib/libhttp_fetcher.*)

ifneq ($(strip $(have_httpfetcher)),)
	CXXFLAGS += -DUSE_HTTP_FETCHER
endif

CPPFLAGS = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUSE_LZO_DATAGRID $(SNR_CPPFLAGS)
LDFLAGS =

ifneq ($(strip $(have_httpfetcher)),)
	LDLIBS += -lhttp_fetcher
endif

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

header=*.h


sources = WindowUtils.cpp

all: libWindowUtils.so all-recursive

libWindowUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../lib
	cp $@ ../lib

libWindowUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

distclean: clean

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libWindowUtils.so libWindowUtils.a >/dev/null

docs:
	doxygen Doxyfile

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)pe

install: 
	mkdir -p $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include/lmce
	cp libWindowUtils.so $(DESTDIR)/usr/pluto/lib
	cp $(header) $(DESTDIR)/usr/include/lmce
