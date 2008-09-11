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

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r -lX11 -lXmu
#<-mkr_B_use_mysql_wrapper_e->

#

sources = WindowUtils.cpp

all: libWindowUtils.so all-recursive

libWindowUtils.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../lib

libWindowUtils.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libWindowUtils.so libWindowUtils.a >/dev/null

docs:
	doxygen Doxyfile

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)pe
