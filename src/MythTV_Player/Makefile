#<-dceag-mh-b->!
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3 -fmessage-length=0
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = pthread
INTERNAL_LIBS_NAMES = SerializeClass DCECommon PlutoUtils WindowUtils

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDFLAGS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDFLAGS += -lmysqlclient_r
endif

MYNAME = MythTV_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 35mplateID

Gen_Dev_Files = $(GENDEV)/MythTV_PlayerBase.cpp
sources = Main.cpp MythTV_Player.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS += -L/usr/X11R6/lib
INTERNAL_LIBS_NAMES +=
EXTERNAL_LIBS_NAMES += Xtst X11

Gen_Dev_Files +=
sources += \
	../utilities/linux/ImageConvertor.cpp

#<-dceag-mf-b->!
LDLIBS_NAMES = $(INTERNAL_LIBS_NAMES) $(EXTERNAL_LIBS_NAMES)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: MythTV_Player

so: MythTV_Player.so

MythTV_Player.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	mkdir -p ../bin
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

MythTV_Player: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	mkdir -p ../bin
	cp $@ ../bin
	./post_make.sh $@

distclean: clean

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) MythTV_Player.bin MythTV_Player.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp MythTV_Player $(DESTDIR)/usr/pluto/bin
	cp TuneToChannel.sh $(DESTDIR)/usr/pluto/bin
	cp launchMythSetup.sh $(DESTDIR)/usr/pluto/bin
