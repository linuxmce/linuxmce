CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I../../../../.. -I/usr/X11R6/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I../../../../DCE -DDEBUG -DTEST_UNIT
LDFLAGS= -L/usr/X11R6/lib -L../../../../../lib
LDLIBS= -lSM -lICE -lX11 -lXmu  -lglib-2.0 -lPlutoUtils -lDCECommon

sources = \
	../../wmctrl/wmctrl.cpp \
	../../../wrapper/image_file.cpp \
	../WMController.cpp \
	../../../wrapper/wrapper_x11.cpp \
	Main.cpp 

all: $(sources:.cpp=.o)
	$(CXX) -o plutowmctrl $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f plutowmctrl $(sources:.cpp=.o) 
