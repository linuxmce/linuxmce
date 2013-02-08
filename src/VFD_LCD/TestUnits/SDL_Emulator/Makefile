ROOT=../../..

CXX=g++
CC=$(CXX)
CXXFLAGS=-W -Wall -pipe -ggdb3
CPPFLAGS=-I. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/SDL -I/usr/include/libxml2/ -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib -lDCECommon -lPlutoUtils -lxml2 -lSDL -lSDL_ttf -lpluto_main -lSerial

sources = \
	  ../../LCDLogic/ActionProcessor.cpp \
	  ../../LCDLogic/InputMenuItem.cpp \
	  ../../LCDLogic/LCDManager.cpp \
	  ../../LCDLogic/MenuHolder.cpp \
	  ../../LCDLogic/MenuItem.cpp \
	  ../../LCDLogic/MenuItemAction.cpp \
	  ../../LCDLogic/MenuLoader.cpp \
	  ../../Renderers/LCDRenderer.cpp \
	  SDLFrontEnd.cpp \
	  ../../InputProviders/SocketStatusInputProvider.cpp \
	  main.cpp
			
all: $(sources:.cpp=.o)
	$(CXX) -o SDL_Emulator $+ $(LDFLAGS)
	cp SDL_Emulator ../../../bin/
clean:
	rm -f SDL_Emulator $(sources:.cpp=.o) 
	
