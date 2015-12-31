# Makefile for ownet example program -- invoked separately from this directory
# $Id: Makefile.in,v 1.3 2009/03/24 17:20:09 alfille Exp $

CFLAGS = -g -I/usr/include

EXAMPLEA = ownetexample
EXAMPLEA_OBJS = ownetexample.o
EXAMPLEB = ownet_tree
EXAMPLEB_OBJS = ownet_tree.o

all:	$(EXAMPLEA) $(EXAMPLEB)

ifeq "$(shell uname)" "Darwin"

# Compile-flags for MacOSX
DARWINLDFLAGS = -L/usr/lib -lownet -low -L/usr/lib -lusb

$(EXAMPLEA): $(EXAMPLEA_OBJS)
	gcc $(CFLAGS) -o $@ $(EXAMPLEA_OBJS) $(DARWINLDFLAGS)

$(EXAMPLEB): $(EXAMPLEB_OBJS)
	gcc $(CFLAGS) -o $@ $(EXAMPLEB_OBJS) $(DARWINLDFLAGS)

else

# Compile-flags for Linux and Cygwin
LDFLAGS = -L/usr/lib -Wl,--rpath -Wl,/usr/lib -lownet

$(EXAMPLEA): $(EXAMPLEA_OBJS)
	gcc $(CFLAGS) -o $@ $(EXAMPLEA_OBJS) $(LDFLAGS)

$(EXAMPLEB): $(EXAMPLEB_OBJS)
	gcc $(CFLAGS) -o $@ $(EXAMPLEB_OBJS) $(LDFLAGS)

endif

%.o: %.c
	i486-linux-gnu-gcc $(CFLAGS) -c -o $@ $<

clean:
	$(RM) -f $(EXAMPLEA) $(EXAMPLEB) *.o *~ .~ Makefile
