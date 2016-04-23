#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS = -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS = 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I./include $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
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

MYNAME = DLNA
VERSION = 2.0.0
MYDEVTEMPLATEID = 2215mplateID

Gen_Dev_Files = $(GENDEV)/DLNABase.cpp
sources = Main.cpp DLNA.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/qt4/ 
LDFLAGS +=
XTRALDLIBS = -lQtGui -lQtCore -lHUpnp -lHUpnpAv -lpluto_main -lQtNetwork

Gen_Dev_Files +=
sources += DLNAEngine.cpp LMCERenderer.cpp LMCERendererConnectionManager.cpp moc_LMCERenderer.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: DLNA

so: DLNA.so

moc_LMCERenderer.cpp:
	QT_SELECT=4 moc -o moc_LMCERenderer.cpp LMCERenderer.h

DLNA.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

DLNA: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

distclean: clean

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) moc_LMCERenderer.cpp DLNA.bin DLNA.so >/dev/null

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
	cp DLNA $(DESTDIR)/usr/pluto/bin
