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

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = General_Info_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 27mplateID

Gen_Dev_Files = $(GENDEV)/General_Info_PluginBase.cpp
sources = Main.cpp General_Info_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
#-DWEB_QUERY_DEBUG -DDEBUG
LDFLAGS +=
XTRALDLIBS = -lpluto_main -lpluto_media

Gen_Dev_Files += 
sources += Web_DHCP_Query.cpp \
	PostCreateOptions.cpp \
	../CreateDevice/CreateDevice.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: General_Info_Plugin

so: General_Info_Plugin.so

General_Info_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	chmod +x post_make.sh
#	./post_make.sh $@
		
General_Info_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) General_Info_Plugin.bin General_Info_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp General_Info_Plugin.so $(DESTDIR)/usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

Web_DHCP_Query_test: $(sources:.cpp=.o)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o Web_DHCP_Query_test Web_DHCP_Query.o Web_DHCP_Query_test.cpp $(LDFLAGS) $(LDLIBS)

