SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ../../..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -DDEBUG $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib

MYNAME = AVWizard

CXXFLAGS += -g -O0 -pipe
CPPFLAGS += -I/usr/include/SDL -I/usr/include/libxml2 
LDLIBS = -lSDL -lSDL_ttf -lSDL_image -lxml2 -lxine -lSDL_gfx -lGL -lresolution -lpthread
FLAGS = $(CXXFLAGS) $(CPPFLAGS)
Gen_Dev_Files +=
include sources.list.mk
    
AVWizard: $(sources:.cpp=.o)
	$(CXX) -o AVWizard $+ $(FLAGS) $(LDFLAGS) $(LDLIBS)
	cp AVWizard ../../../bin/

clean: 
	rm -f *.o *.d AVWizard 

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
