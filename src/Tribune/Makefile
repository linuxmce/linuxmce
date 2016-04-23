ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -D_REENTRANT -D_GNU_SOURCE -D_DEVEL_X_DEFINES -DGENERATOR
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lRACommon -lDCECommon -lPlutoUtils -lSerializeClass -lpthread #-lattr 
CXX = g++

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources = CommonFunctions.cpp Tribunerequests.cpp R_GetLineups.cpp \
	R_GetChannels.cpp R_GetDiffs.cpp ClientFunctions.cpp \
	Database.cpp Tribuneprocessor.cpp ServerManagement.cpp \
	MapManagement.cpp Tribune.cpp 
#	$(ROOT)/inotify/FileNotifier.cpp $(ROOT)/inotify/inotify_class.cpp

#new_thing = $(sources) TableInfo_SCVS.cpp

all: Tribune
#all: newthing

Tribune: $(sources:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS) $(CPPFLAGS)
	cp $@ $(ROOT)/bin

#newthing: $(new_thing:.cpp=.o)
#	@echo "*** Something to do here ***"

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Tribune >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
