#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = Security_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 33mplateID

Gen_Dev_Files = $(GENDEV)/Security_PluginBase.cpp
sources = Main.cpp Security_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS += 
XTRALDLIBS = -lpluto_main -lpluto_security

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

Gen_Dev_Files +=
sources += Notification.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Security_Plugin

so: Security_Plugin.so

Security_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Security_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Security_Plugin.bin Security_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /home/public
	cp bookmarks.html $(DESTPATH)/home/public
	mkdir -p /usr/pluto/bin
	cp Datagrid_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp File_Grids_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Security_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Climate_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Orbiter_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Media_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Telecom_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Lighting_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp General_Info_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Event_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp Infrared_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp RestartSecurityPlugin.sh $(DESTPATH)/usr/pluto/bin
	cp Plug_And_Play_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp flickr.pl $(DESTPATH)/usr/pluto/bin
	cp *.php $(DESTPATH)/usr/pluto/bin
	cp flickr_status.sh $(DESTPATH)/usr/pluto/bin
