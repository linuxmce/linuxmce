TS = ../Splitter
ORB = ../Orbiter

SNR_LDFLAGS ?= <-mkr_t_compile_libs->

ROOT = ..
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I. -I/usr/include/SDL -I$(TS) -I$(ROOT)/SDL_Helpers -DOrbiterGen
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
LDLIBS = -ldl -lpluto_main -lpluto_media -lDCECommon -lPlutoUtils -lSerializeClass -lSDL -lSGE -lSDL_gfx -lSDL_image -lSDL_ttf -lGL -lXrender -lSDL_Helpers

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#<-mkr_b_use_mysql_wrapper_b->
CPPFLAGS += -DUSE_MYSQL_WRAPPER
#<-mkr_b_use_mysql_wrapper_e->

APPNAME = OrbiterGen
VERSION = 2.0.0

sources = \
	../utilities/linux/wrapper/image_file.cpp \
	OrbiterGen.cpp \
	OGCommand.cpp \
	OGArray.cpp \
	DesignObj_Generator.cpp \
	OGText.cpp Renderer.cpp \
	RegenMonitor.cpp \
	ScreenMap.cpp \
	$(TS)/TextSplitter.cpp \
	$(TS)/TextWrapper.cpp \
	$(ORB)/SDL/SDLRendererOCGHelper.cpp \
	$(ORB)/RendererOCG.cpp \
	$(ORB)/DesignObj_Data.cpp \
	$(ORB)/TextStyle.cpp \
	$(ORB)/RendererMNG.cpp

all: bin

bin: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(APPNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp OrbiterGen ../bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) $(APPNAME) >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
