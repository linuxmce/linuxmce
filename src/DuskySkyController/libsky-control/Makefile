BASENAME = sky-control
VER_MAJOR = 1
VER_MINOR = 0
VER_BUILD = 1

OBJS = \
	sky-control.o\
	gnome.o\
	serial.o\
	buffer.o\
	sky-serial.o\
	sky-control-lib.o

# Make symbolic link names
LINKNAME = lib$(BASENAME).so
SONAME = $(LINKNAME).$(VER_MAJOR)
LIBNAME = $(SONAME).$(VER_MINOR).$(VER_BUILD)

LIB = lib$(BASENAME).a

CC = gcc
LD = ld
AR = ar
NM = nm
RANLIB = ranlib
STRIP = strip

.c.o:
	$(CC) -g -Wall -c -DENABLE_USB $<

$(LIBNAME): $(OBJS)
	$(LD) -shared -soname $(SONAME) -o $(LIBNAME) $(OBJS) -lusb -lc
	$(STRIP) $(LIBNAME)
	ln -sf $(LIBNAME) $(SONAME)
	ln -sf $(SONAME) $(LINKNAME)

$(LIB): $(OBJS)
	$(AR) rv $@ $?
	$(RANLIB) $@

all: $(LIBNAME) $(LIB)

clean:
	-rm -f *.o $(LIBNAME) $(LIB) $(SONAME) $(LINKNAME)
