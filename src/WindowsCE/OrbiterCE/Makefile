BaseDir=..

CPPFLAGS=-I $(BaseDir)/zlib-1.2.5 -I $(BaseDir)/libpng-1.4.3 -I $(BaseDir)/jpeg-6b-src/src/jpeg-6b-src -I $(BaseDir)/jpeg-6b-src/src/jpeg-6b
CPPFLAGS+=-D_WIN32_WCE=0x0500
CFLAGS=-W -Wall
LDFLAGS=-L$(BaseDir)/zlib-1.2.5 -L$(BaseDir)/libpng-1.4.3 -L$(BaseDir)/jpeg-6b-src/src/jpeg-6b-src
LDLIBS=-lpng -lz -lcommctrl -lws2 -laygshell -limgdecmp -ljpeg

export PATH:=/opt/mingw32ce/bin:${PATH}

%.rsc: %.rc
	arm-mingw32ce-windres $< $@

%.o: %.c
	arm-mingw32ce-gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

Objects=OrbiterCE.o OrbiterCE.rsc PngFile.o JpegFile.o ProxyOrbiter.o TransformBitmap.o WaitingCursor.o IniFile.o

OrbiterCE.exe: $(Objects)
	arm-mingw32ce-gcc $(CFLAGS) -o $@ $+ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f OrbiterCE.exe *.o *.rsc
