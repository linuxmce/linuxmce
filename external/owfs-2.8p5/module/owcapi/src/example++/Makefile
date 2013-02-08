# Makefile for owcapi example program -- invoked separately from this directory
# $Id: Makefile.in,v 1.2 2009/03/24 17:20:09 alfille Exp $

CXXFLAGS = -g -I/usr/include -Wno-deprecated

OBJS = owcapiexample.o

all:	owcapiexample

ifeq "$(shell uname)" "Darwin"

# Compile-flags for MacOSX
DARWINLDFLAGS = -L/usr/lib -lowcapi -low -L/usr/lib -lusb

owcapiexample: $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS) $(DARWINLDFLAGS)

else

# Compile-flags for Linux and Cygwin
LDFLAGS = -L/usr/lib -Wl,--rpath -Wl,/usr/lib -lowcapi

owcapiexample: $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) -lstdc++

endif

#%.o: %.cpp
#	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) -f owcapiexample *.o *~ .~ Makefile
