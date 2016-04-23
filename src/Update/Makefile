GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS) `xml2-config --cflags`
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib `xml2-config --libs`
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = LMCEUpdate
VERSION = 1.0.0

sources = Main.cpp

CPPFLAGS +=
CXXFLAGS +=
LDFLAGS +=
XTRALDLIBS =

Gen_Dev_Files +=
sources += UpdatesXML.cpp UpdatesManager.cpp

LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin' or 'make all'"

all: bin

bin: LMCEUpdate

LMCEUpdate: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d))  >/dev/null

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p /etc/cron.d
	cp * $(DESTPATH)/etc/cron.d
	mkdir -p /etc/init.d
	cp * $(DESTPATH)/etc/init.d
	mkdir -p /usr/pluto/bin
	cp LMCEUpdate $(DESTPATH)/usr/pluto/bin
	cp LMCEUpdate_*.sh $(DESTPATH)/usr/pluto/bin
	mkdir -p src/Update
	cp mkr_*.sh $(DESTPATH)src/Update
