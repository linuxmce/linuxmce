SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE
CPPFLAGS = -I./ -I$(ROOT) -I/usr/include/SDL -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE $(SNR_CPPFLAGS)
CXX = g++
LDFLAGS =
LDLIBS = $(SNR_LDFLAGS) -lSDL -lpng -lSDL_gfx -lSDL_image

sources = \
	SDL_CompositeAlphaChannel.cpp \
	SDL_pixel.cpp \
	SDL_SavePNG.cpp \
	SDL_RenderText.cpp \
	SDL_RenderUtils.cpp 
	
headers = *.h
	
all: libSDL_Helpers.so

libSDL_Helpers.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../lib
	cp $@ ../lib
	
	
libSDL_Helpers.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

distclean: clean
	
clean: 
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libSDL_Helpers.so libSDL_Helpers.a >/dev/null

install: 
	mkdir -p $(DESTDIR)/usr/lib $(DESTDIR)/usr/include
	cp libSDL_Helpers.so $(DESTDIR)/usr/lib
	cp $(headers) $(DESTDIR)/usr/include

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
