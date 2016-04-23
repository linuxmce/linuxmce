ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib
sources = Database_pluto_media.cpp TableRow.cpp

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources +=  \
Table_Attribute.cpp \
Table_AttributeType.cpp \
Table_Attribute_Settings.cpp \
Table_Bookmark.cpp \
Table_CoverArtScan.cpp \
Table_CoverArtScanEntry.cpp \
Table_Disc.cpp \
Table_DiscLocation.cpp \
Table_Disc_Attribute.cpp \
Table_Disc_Users.cpp \
Table_Download.cpp \
Table_DownloadSource.cpp \
Table_Download_Attribute.cpp \
Table_File.cpp \
Table_FileFormat.cpp \
Table_FileGroup.cpp \
Table_File_Attribute.cpp \
Table_File_Users.cpp \
Table_LongAttribute.cpp \
Table_MediaProvider.cpp \
Table_MediaSource.cpp \
Table_MediaSubType.cpp \
Table_MediaType_AttributeType.cpp \
Table_MediaType_FileFormat.cpp \
Table_MediaType_MediaSource.cpp \
Table_MediaType_MediaSubType.cpp \
Table_Picture.cpp \
Table_Picture_Attribute.cpp \
Table_Picture_Disc.cpp \
Table_Picture_Download.cpp \
Table_Picture_File.cpp \
Table_Playlist.cpp \
Table_PlaylistEntry.cpp \
Table_ProviderSource.cpp \
Table_RipStatus.cpp \
Table_SearchToken.cpp \
Table_SearchToken_Attribute.cpp \
Table_psc_media_batdet.cpp \
Table_psc_media_bathdr.cpp \
Table_psc_media_batuser.cpp \
Table_psc_media_repset.cpp \
Table_psc_media_schema.cpp \
Table_psc_media_tables.cpp

all: libpluto_media.so all-recursive

libpluto_media.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_media.so >/dev/null

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp $@ /usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
