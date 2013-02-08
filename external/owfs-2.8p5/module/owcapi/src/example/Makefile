# Makefile for owcapi example program -- invoked separately from this directory
# $Id: Makefile.in,v 1.11 2009/03/24 17:20:08 alfille Exp $

CFLAGS = -g -I/usr/include

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
	gcc $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

endif

%.o: %.c
	i486-linux-gnu-gcc $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -f owcapiexample *.o *~ .~ Makefile
