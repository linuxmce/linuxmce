CC=gcc
CFLAGS ?= -g -Wall -O2
CFLAGS += $(ARCH)
CPPFLAGS += -MD -I../../include -I../lib -I. $(DEFS)

OBJS = filter.o pat.o pmt.o si.o descriptor.o channels.o
all: $(OBJS) libs

$(OBJS): %.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.d *.a

libs: $(OBJS)
	ar rcs libdvbsi.a filter.o pat.o pmt.o si.o descriptor.o channels.o

install:

-include $(wildcard *.d) dummy
