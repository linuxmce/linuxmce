ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib
sources = Database_lmce_game.cpp TableRow.cpp

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources +=  \
Table_Configuration.cpp \
Table_GameSystem.cpp \
Table_Rom.cpp \
Table_RomAttribute.cpp \
Table_RomAttributeType.cpp \
Table_RomTitles.cpp \
Table_Rom_Configuration.cpp \
Table_Rom_RomAttribute.cpp \
Table_psc_game_batdet.cpp \
Table_psc_game_bathdr.cpp \
Table_psc_game_batuser.cpp \
Table_psc_game_repset.cpp \
Table_psc_game_schema.cpp \
Table_psc_game_tables.cpp

all: liblmce_game.so all-recursive

liblmce_game.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d liblmce_game.so >/dev/null

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp $@ /usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
