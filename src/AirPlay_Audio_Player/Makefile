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
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = AirPlay_Audio_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 2188

Gen_Dev_Files = $(GENDEV)/AirPlay_Audio_PlayerBase.cpp
sources = Main.cpp AirPlay_Audio_Player.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lm $(shell pkg-config --libs openssl ao)

Gen_Dev_Files +=
sources += 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: AirPlay_Audio_Player

so: AirPlay_Audio_Player.so

AirPlay_Audio_Player.so: $(sources:.cpp=.o)
	cc -O2 -Wall -c socketlib.c -o socketlib.o
	cc -O2 -Wall -c alac.c -o alac.o
	cc -O2 -Wall -c hairtunes.c -o hairtunes.o

	$(CXX) -shared -o $(MYNAME).so socketlib.o hairtunes.o alac.o $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
AirPlay_Audio_Player: $(sources:.cpp=.o)
	cc -O2 -Wall -c socketlib.c -o socketlib.o
	cc -O2 -Wall -c alac.c -o alac.o
	cc -O2 -Wall -c hairtunes.c -o hairtunes.o
	
	$(CXX) -o $(MYNAME) socketlib.o hairtunes.o alac.o $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) AirPlay_Audio_Player.bin AirPlay_Audio_Player.so >/dev/null
	rm -f hairtunes.o alac.o socketlib.o >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /usr/pluto/bin
	cp AirPlay_Audio_Player $(DESTPATH)/usr/pluto/bin
