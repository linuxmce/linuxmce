#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ../..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3 -std=c++11
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = import-mame
VERSION = 2.0.0

sources = ImportMame.cpp \
	Category.cpp \
	Database.cpp \
        MAMEXMLParser.cpp \
	NameHash.cpp \
	pugixml.cpp \
	main.cpp
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += 
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r -llmce_game

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

all: bin

bin: import-mame

import-mame: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) import-mame >/dev/null

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->
