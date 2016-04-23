#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine1-pluto/include -I/opt/linphone-1.3.5/include -DVERSION="\"$(COMPILE_VERSION)\"" -DCOMPILE_DATE="\"$(COMPILE_DATE)\""
SNR_LDFLAGS ?= -L/opt/libxine1-pluto/lib -L/opt/linphone-1.3.5/lib

ROOT = ..
CXX = g++
CFLAGS = -Wall -fPIC -ggdb3
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I /usr/include/pluto -I /usr/include/pluto/DCE -I$(ROOT) $(SNR_CPPFLAGS) -I../DCE
LDFLAGS = -L /usr/pluto/lib -L$(ROOT)/lib
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

MYNAME = JSON_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 2294mplateID

Gen_Dev_Files = $(GENDEV)/JSON_PluginBase.cpp
sources = Main.cpp JSON_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

sources += civetweb/CivetServer.cpp \
	   CivetHandler/NowPlayingHandler.cpp \
	   CivetHandler/MediaPlaylistHandler.cpp \
	   EscapeJS.cpp
Csources = civetweb/civetweb.c \
	   base64.c

CFLAGS += --std=gnu99 -DNO_SSL -DNO_SSL_DL -DNO_CGI
CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lrt

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b-!>
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: JSON_Plugin

so: JSON_Plugin.so

JSON_Plugin.so: $(sources:.cpp=.o) $(Csources:.c=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
JSON_Plugin: $(sources:.cpp=.o) $(Csources.c=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d) $(Csources:.c=.o) $(Csources:.c=.d)) JSON_Plugin.bin JSON_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
-include $(Csources:.c=.d)
endif
#<-dceag-mf-e->

