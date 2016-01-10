ROOT = ..
CXXFLAGS = -Wall -fmessage-length=0 -g -I./ `pkg-config --cflags glib-2.0` -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -I$(ROOT)/Media_Plugin 
LDFLAGS = -L. -L$(ROOT)/lib
LDLIBS = -fmessage-length=0 `pkg-config --libs glib-2.0` -lattr -lpthread -lDCECommon -lpluto_media -lSerializeClass -lPlutoUtils -lmysqlclient_r
MY_DEVICE = medialibd
SOURCES = main.cpp \
	Controller.cpp \
	Directory.cpp \
	Socket.cpp ServerSocket.cpp \
	$(ROOT)/Media_Plugin/MediaAttributes.cpp

all: $(MY_DEVICE) medialib

medialib: MediaLib.cpp MediaLib.h MediaLib.o
	$(CXX) -o medialib MediaLib.o

$(MY_DEVICE): $(SOURCES:.cpp=.o)
	$(CXX) -o $(MY_DEVICE) $(call cpath,$(SOURCES:.cpp=.o)) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(call cpath,$(SOURCES:.cpp=.o)) $(MY_DEVICE) medialib

include $(ROOT)/PlutoUtils/make.rules
