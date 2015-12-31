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

MYNAME = Disk_Drive
VERSION = 2.0.0
MYDEVTEMPLATEID = 11mplateID

Gen_Dev_Files = $(GENDEV)/Disk_DriveBase.cpp
sources = Main.cpp Disk_Drive.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lcddb -lDisk_Drive_Functions -lpluto_media

Gen_Dev_Files +=
sources += ../JobHandler/Job.cpp ../JobHandler/JobHandler.cpp ../JobHandler/Task.cpp
#FIXME: refactor for udev to support local jukeboxes again
#sources += ../HAL/HalTree.cpp

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Disk_Drive

so: Disk_Drive.so

Disk_Drive.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Disk_Drive: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Disk_Drive.bin Disk_Drive.so >/dev/null

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
	cp Disk_Drive $(DESTPATH)/usr/pluto/bin
	cp mountDVDCommand.sh $(DESTPATH)/usr/pluto/bin
	cp ripDiskWrapper.sh $(DESTPATH)/usr/pluto/bin
	cp Paranoia_Progress.sh $(DESTPATH)/usr/pluto/bin
	cp disk_copy $(DESTPATH)/usr/pluto/bin
	cp Pluto_Progress.sh $(DESTPATH)/usr/pluto/bin
	cp DiskCopy_ProgressExtract.sh $(DESTPATH)/usr/pluto/bin
	cp disc_unlock $(DESTPATH)/usr/pluto/bin
	cp cdop $(DESTPATH)/usr/pluto/bin
	cp dvd_region $(DESTPATH)/usr/pluto/bin
	cp IsDataTrack $(DESTPATH)/usr/pluto/bin
	cp dvd_unique_id $(DESTPATH)/usr/pluto/bin
	cp dvdcssHelper $(DESTPATH)/usr/pluto/bin
	cp dvdcssHelper.sh $(DESTPATH)/usr/pluto/bin
	cp fixDVDKeys.sh $(DESTPATH)/usr/pluto/bin
