GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS) `xml2-config --cflags`
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib `xml2-config --libs`
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

MYNAME = LMCEUpdate
VERSION = 1.0.0

sources = Main.cpp

CPPFLAGS +=
CXXFLAGS +=
LDFLAGS +=
XTRALDLIBS =

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

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
