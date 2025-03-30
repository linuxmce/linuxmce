#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3 -I/usr/include/linphone
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS) -I/usr/include/linphone
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -llinphone

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = SimplePhone
VERSION = 2.0.0
MYDEVTEMPLATEID = 1759

Gen_Dev_Files = $(GENDEV)/SimplePhoneBase.cpp
sources = Main.cpp SimplePhone.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += $(shell pkg-config --cflags glib-2.0)
CPPFLAGS += $(shell pkg-config --cflags glib-2.0) -DHAVE_CONFIG_H -DLINPHONE_CONST=$(LINPHONE_CONST)
LDFLAGS += $(shell pkg-config --libs-only-L glib-2.0)
XTRALDLIBS = $(shell pkg-config --libs-only-l glib-2.0)

Gen_Dev_Files +=
sources += SIP_Thread.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: SimplePhone

so: SimplePhone.so

SimplePhone.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	chmod +x post_make.sh
#	./post_make.sh $@
		
SimplePhone: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) SimplePhone SimplePhone.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp SimplePhone $(DESTDIR)/usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

install: 
	mkdir -p /usr/pluto/bin
	cp SimplePhone $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/templates
	cp simplephone.conf $(DESTPATH)pluto/templates
