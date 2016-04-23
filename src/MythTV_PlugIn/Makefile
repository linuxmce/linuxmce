#<-dceag-mh-b->!
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..

CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = pthread
INTERNAL_LIBS_NAMES = SerializeClass DCECommon PlutoUtils

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDFLAGS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDFLAGS += -lmysqlclient_r
endif

MYNAME = MythTV_PlugIn
VERSION = 2.0.0
MYDEVTEMPLATEID = 36mplateID

Gen_Dev_Files = $(GENDEV)/MythTV_PlugInBase.cpp
sources = Main.cpp MythTV_PlugIn.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS += -DQT_THREAD_SUPPORT -I/usr/include/qt3 -I$(ROOT)/../libs/Libraries
INTERNAL_LIBS_NAMES += pluto_main pluto_media

Gen_Dev_Files += 
sources += \
	MythTvMediaStream.cpp EPGGrid.cpp MythBackEnd_Socket.cpp

#<-dceag-mf-b->!
LDLIBS_NAMES = $(INTERNAL_LIBS_NAMES) $(EXTERNAL_LIBS_NAMES)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: MythTV_PlugIn

so: MythTV_PlugIn.so

MythTV_PlugIn.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

MythTV_PlugIn: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	./post_make.sh $@

distclean: clean

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) MythTV_PlugIn.bin MythTV_PlugIn.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p $(DESTDIR)/home/mythtv $(DESTDIR)/usr/pluto/bin $(DESTDIR)/usr/pluto/database
	cp master_iconmap.xml $(DESTDIR)/home/mythtv
	cp MythTV_PlugIn.so $(DESTDIR)/usr/pluto/bin
	cp Tribune*.sh $(DESTDIR)/usr/pluto/bin
	cp DataDirect*.sh $(DESTDIR)/usr/pluto/bin
	cp MythTvInitialFillDB.sh $(DESTDIR)/usr/pluto/bin
	cp Restart_Backend_With_SchemaLock.sh $(DESTDIR)/usr/pluto/bin
	cp mkr_*.sh $(DESTDIR)/usr/pluto/bin
	cp MythTvSyncDB.sh $(DESTDIR)/usr/pluto/bin
	cp MythTvDailyFillDB.sh $(DESTDIR)/usr/pluto/bin
	cp MythTvCommon.sh $(DESTDIR)/usr/pluto/bin
	cp mythtv_setup.pl $(DESTDIR)/usr/pluto/bin
	mkdir -p $(DESTDIR)/usr/pluto/database
	cp myth.sqlcvs $(DESTDIR)pluto/database || :
