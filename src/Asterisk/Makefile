#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = Asterisk
VERSION = 2.0.0
MYDEVTEMPLATEID = 45mplateID

Gen_Dev_Files = $(GENDEV)/AsteriskBase.cpp
sources = Main.cpp Asterisk.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
LDLIBS +=
XTRALDLIBS =

Gen_Dev_Files +=
sources += \
	token.cpp \
	thread.cpp \
	threadsafeobj.cpp \
	mutex.cpp \
	condition.cpp \
	socket.cpp \
	asteriskmanager.cpp \
	runtimeconfig.cpp \
	tokenpool.cpp \
	logincommand.cpp \
	originatecommand.cpp \
	applicationcommand.cpp \
	hangupcommand.cpp \
	transfercommand.cpp \
	iaxextensionsstatuscommand.cpp \
	sipextensionsstatuscommand.cpp \
	communicationhandler.cpp \
	callsstatuscommand.cpp \
	utils.cpp 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Asterisk

so: Asterisk.so

Asterisk.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Asterisk: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Asterisk.bin Asterisk.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /etc/asterisk
	cp provider_list.txt $(DESTPATH)/etc/asterisk
	mkdir -p /usr/pluto/bin
	cp Asterisk $(DESTPATH)/usr/pluto/bin
	cp SendVoiceMailEvent.sh $(DESTPATH)/usr/pluto/bin
	cp *.pl $(DESTPATH)/usr/pluto/bin
	cp create_amp_phoneline.sh $(DESTPATH)/usr/pluto/bin
	cp GenerateSCCP.sh $(DESTPATH)/usr/pluto/bin
