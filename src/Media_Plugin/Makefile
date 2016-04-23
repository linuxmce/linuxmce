#<-dceag-mh-b->!
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
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

MYNAME = Media_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 2mplateID

Gen_Dev_Files = $(GENDEV)/Media_PluginBase.cpp
sources = Main.cpp Media_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
INTERNAL_LIBS_NAMES += pluto_main pluto_media
EXTERNAL_LIBS_NAMES += rt

Gen_Dev_Files += 
sources += \
	Media_Plugin_Grids.cpp \
	MediaListGrid.cpp \
	MediaHandlerInfo.cpp \
	MediaAttributes.cpp \
	MediaAttributes_LowLevel.cpp \
	MediaStream.cpp \
	BoundRemote.cpp \
	Generic_NonPluto_Media.cpp \
	MediaHandlerBase.cpp \
	PlaylistParser.cpp \
	../PlutoUtils/HttpUtils.cpp \
	../JobHandler/JobHandler.cpp \
	../JobHandler/Job.cpp \
	../JobHandler/Task.cpp

#<-dceag-mf-b->!
LDLIBS_NAMES = $(INTERNAL_LIBS_NAMES) $(EXTERNAL_LIBS_NAMES)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Media_Plugin

so: Media_Plugin.so

Media_Plugin.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	mkdir -p ../bin
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Media_Plugin: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.a) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	mkdir -p ../bin
	cp $@ ../bin
	./post_make.sh $@

distclean: clean

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Media_Plugin.bin Media_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p $(DESTDIR)/home/public $(DESTDIR)/usr/pluto/bin
	#cp bookmarks.html $(DESTDIR)/home/public
	cp ../bin/Media_Plugin.so $(DESTDIR)/usr/pluto/bin
	#cp flickr.pl $(DESTDIR)/usr/pluto/bin
	#cp *.php $(DESTDIR)/usr/pluto/bin
	#cp flickr_status.sh $(DESTDIR)/usr/pluto/bin
