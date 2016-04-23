ROOT = ..
SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

CXX = g++
CXXFLAGS = -W -Wall -fPIC -ggdb3
CPPFLAGS = -I. -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -lpluto_media 
MYNAME = Disk_Drive_Functions

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources = Disk_Drive_Functions.cpp \
	RipTask.cpp RipJob.cpp IdentifyJob.cpp \
	IdentifyTask.cpp ProcessMediaIdentifiedJob.cpp \
	MoveDiscTask.cpp FixupRippingInfoTask.cpp AddRippingTasksTask.cpp \
	LoadUnloadJob.cpp JukeBox.cpp ../Media_Plugin/MediaAttributes_LowLevel.cpp \
	../JobHandler/JobHandler.cpp \
	../JobHandler/Job.cpp \
	../JobHandler/Task.cpp 

all: $(sources:.cpp=.o)
	$(CXX) -shared -o lib$(MYNAME).so $+ $(LDFLAGS) $(LDLIBS)
	cp lib$(MYNAME).so ../lib/

clean:
	rm -f $(sources:.cpp=.o) *.so

install: 
	mkdir -p pluto/lib
	cp libDisk_Drive_Functions.so $(DESTPATH)pluto/lib
