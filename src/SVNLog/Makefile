SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -I/usr/include/mysql -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DGENERATOR
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lDCECommon -lPlutoUtils -lpthread -lmysqlclient_r -lattr
CXX = g++

APPNAME = SVNLog
VERSION = 2.0.0

sources = SVNLog.cpp
SVNLog = $(sources) 

all: SVNLog

SVNLog: $(SVNLog:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) SVNLog >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
