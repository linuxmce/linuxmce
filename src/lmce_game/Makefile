ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib
sources = Database_lmce_game.cpp TableRow.cpp
#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

sources +=  \
Table_Configuration.cpp \
Table_Game.cpp \
Table_GameSystem.cpp \
Table_GameSystem_Picture.cpp \
Table_Game_GameSystem.cpp \
Table_Game_GameSystem_Configuration.cpp \
Table_Game_GameSystem_Picture.cpp \
Table_Game_GameSystem_Rom.cpp \
Table_Game_GameSystem_Rom_Configuration.cpp \
Table_Genre.cpp \
Table_Manufacturer.cpp \
Table_NameHash.cpp \
Table_Picture.cpp \
Table_Rom.cpp

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
